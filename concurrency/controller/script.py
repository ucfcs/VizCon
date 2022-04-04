from time import sleep
from thread_manager import ThreadManager
import lldb
import json
import base64
import os
import sys
import traceback

def start(exe, visualizerMode):
    try:
        _start(base64.b64decode(exe).decode(), visualizerMode)
    except SystemExit as e:
        # Exit immediately to bypass LLDB catching the SystemExit
        sys.stdout.flush()
        sys.stderr.flush()
        os._exit(e.code)
    except Exception as e:
        traceback.print_exc()
    finally:
        sys.stdout.flush()
        sys.stderr.flush()
        os._exit(0)
def _start(exe, visualizerMode):
    if not visualizerMode:
        dataOutputFile = sys.stdout
    else:
        dataOutputFile = os.fdopen(3, 'w')

    # TODO
    def debug_print(*args, **kwargs):
        return
        #print(*args, file=sys.stderr, **kwargs)
        #sys.stderr.flush()
        #sys.stdout.flush()

    def waitForVisualizer():
        if not visualizerMode:
            return
        lineIn = sys.stdin.readline()
        objIn = json.loads(lineIn)
    def respondToVisualizer(msg):
        dataOutputFile.write(json.dumps(msg))
        dataOutputFile.write("\n")
        dataOutputFile.flush()
    def isUserCode(thread):
        frame = thread.GetFrameAtIndex(0)
        line = frame.GetLineEntry()
        if not line.IsValid():
            return False

        if target.GetExecutable() != frame.GetModule().GetFileSpec():
            return False
        file = line.GetFileSpec().GetFilename()
        # TODO: keep this updated
        if file is None:
            return False
        if "lldb_lib.c" in file:
            return False
        full_file = str(line.GetFileSpec())
        if "mingw-w64" in full_file or "mingw64" in full_file:
            return False
        if "vcuserlibrary" in file:
            return False
        if "threads.c" in file or "semaphore.c" in file or "mutexes.c" in file:
            return False
        #print("File is user code", file, str(line.GetFileSpec()), file=sys.stderr)
        return True

    os.environ['LLDB_LAUNCH_INFERIORS_WITHOUT_CONSOLE'] = str(True)
    # Create a new debugger instance
    debugger = lldb.SBDebugger.Create()

    # When we step or continue, don't return from the function until the process 
    # stops. Otherwise we would have to handle the process events ourselves which, while doable is
    #a little tricky.  We do this by setting the async mode to false.
    debugger.SetAsync(False)

    # Create a target from a file and arch
    debug_print("Creating a target for " + exe)

    #target = debugger.CreateTargetWithFileAndArch(exe, lldb.LLDB_ARCH_DEFAULT)
    target = debugger.CreateTarget(exe)

    verbose = False

    if not target:
        debug_print("Error creating target")
        sys.exit(2)

    # If the target is valid set a breakpoint at main
    main_bp = target.BreakpointCreateByName ("userMain", target.GetExecutable().GetFilename())
    thread_bp = target.BreakpointCreateByName ("do_post", target.GetExecutable().GetFilename())
    vcJoin_bp = target.BreakpointCreateByName ("vcJoin", target.GetExecutable().GetFilename())
    vc_internal_registerSem_bp = target.BreakpointCreateByName ("vc_internal_registerSem", target.GetExecutable().GetFilename())

    vcWait_bp = target.BreakpointCreateByName ("vcWait", target.GetExecutable().GetFilename())
    vcSignal_bp = target.BreakpointCreateByName ("vcSignal", target.GetExecutable().GetFilename())
    hook_createThread_bp = target.BreakpointCreateByName ("lldb_hook_createThread", target.GetExecutable().GetFilename())

    launch_info = target.GetLaunchInfo()

    error = lldb.SBError()
    #print(launch_info.AddOpenFileAction(0, "CONIN$", True, False))
    #print(launch_info.AddOpenFileAction(1, "stdout.txt", False, True))
    #print(launch_info.AddOpenFileAction(2, "stderr.txt", False, True))
    launch_info.SetEnvironmentEntries(["lldbMode=1"], True)
    process = target.Launch(launch_info, error)
    debug_print("Error", error.Success(), error, error.GetCString())

    for t in process:
        if isStoppedForBreakpoint(t, main_bp):
            debug_print("Main thread:", t)
            # TODO: probably get an ID for the thread
            main_thread = t

    thread_man = ThreadManager(main_thread)

    running_thread = main_thread
    chosen_cthread = None
    def getThreads():
        for thread in thread_man.getManagedThreads():
            yield thread['thread']
    def serializeVariable(thread_man, lldb_var):
        var_value = lldb_var.GetValue()
        if lldb_var.GetTypeName() == 'CSSem *':
            var_value = thread_man.getSemaphoreValue(str(var_value))    
        return {'name': lldb_var.GetName(), 'type': lldb_var.GetTypeName(), 'value': var_value}

    ignore_set = set()
    respondToVisualizer({'type': 'hello'})
    while True:
        if process.state == lldb.eStateExited:
            debug_print("Process state is", process)
            break

        waitForVisualizer()
        chosen_cthread = thread_man.chooseThread()
        
        if chosen_cthread is None:
            debug_print("Ready list is empty! No more runnable threads! Deadlock?")
            respondToVisualizer({'type': 'error', 'error': 'ready list is empty'})
            sys.exit(1)
        running_thread = chosen_cthread['thread']

        for t in getThreads():
            t.Suspend()

        running_thread.Resume()
        if running_thread.GetThreadID() in ignore_set:
            ignore_set.remove(running_thread.GetThreadID())

        #debug_print("Before stepInstruction", chosen_cthread['name'])
        running_thread.StepInstruction(False)
        current_function = running_thread.GetFrameAtIndex(0).GetFunction().GetDisplayName()
        #debug_print("After stepInstruction", current_function, chosen_cthread['name'])
        if current_function == 'vc_internal_thread_wrapper':
            # TODO: much more cleanup is needed!
            # for example, remove it from managed threads
            # it probably can't be in ignore set but remove it from that anyway
            # and then resume it so it can terminate itself
            # if you do resume it, I dunno what to do about the stop reason handler
            # probably it should be modified to avoid unmanaged threads though
            thread_man.onExitThread(running_thread)
            
            # TODO: think about this, but I think conveniently
            # stepover does not stop othe threads, so we don't actually have to worry about it
            running_thread.Resume()
            # TODO: it might be appropriate to resume all the threads that were waiting
            # but it probably doesn't matter
        elif current_function == 'main':
            # TODO: what if other threads are still running?
            debug_print("Main function exited.")
            # Allow the process to finish (allows cleanup to run)
            for t in process:
                t.Resume()
            process.Continue()
            respondToVisualizer({'type': 'process_end', 'code': process.GetExitStatus()})
            sys.exit(0)
        else:
            if not isUserCode(running_thread):
                debug_print("Special stepout from", running_thread.GetFrameAtIndex(0))
                running_thread.StepOut()
        handlingBreakpoints = True
        while handlingBreakpoints:
            handledBreakpoint = False
            for t in process:
                if isStoppedForBreakpoint(t, vc_internal_registerSem_bp):
                    new_sem = t.GetFrameAtIndex(0).FindVariable("sem").GetValue()
                    new_sem_initial_value = t.GetFrameAtIndex(0).FindVariable("initialValue").GetValueAsSigned()
                    new_sem_max_value = t.GetFrameAtIndex(0).FindVariable("maxValue").GetValueAsSigned()
                    #debug_print("Registering new semaphore:", new_sem, new_sem_name, new_sem_initial_value, new_sem_max_value)
                    thread_man.registerSem(str(new_sem), new_sem_initial_value, new_sem_max_value)
                    process.Continue()
                    handledBreakpoint = True
                    continue
                if isStoppedForBreakpoint(t, vcWait_bp):
                    new_sem = t.GetFrameAtIndex(0).FindVariable("sem").GetValue()
                    thread_man.onWaitSem(t, str(new_sem))
                    # TODO: Add a better resume to replace this
                    process.Continue()
                    handledBreakpoint = True
                    continue
                if isStoppedForBreakpoint(t, vcSignal_bp):
                    new_sem = t.GetFrameAtIndex(0).FindVariable("sem").GetValue()
                    thread_man.onSignalSem(t, str(new_sem))
                    process.Continue()
                    handledBreakpoint = True
                    continue
                if isStoppedForBreakpoint(t, vcJoin_bp):
                    if t.GetThreadID() in ignore_set:
                        if verbose:
                            debug_print("Ignoring a thread")
                        continue
                    thread_val = t.GetFrameAtIndex(0).FindVariable("thread").GetValue()
                    ignore_set.add(t.GetThreadID())
                    thread_man.onJoin(t, thread_val)
                if isStoppedForBreakpoint(t, hook_createThread_bp):
                    new_thread_ptr = t.GetFrameAtIndex(0).FindVariable("thread").GetValue()
                    new_thread_name_ptr = t.GetFrameAtIndex(0).FindVariable("name")
                    new_thread_name = process.ReadCStringFromMemory(new_thread_name_ptr.GetValueAsUnsigned(), 1024, lldb.SBError())
                    #debug_print("New thread", new_thread_ptr)
                    running_thread.Resume()
                    process.Continue()
                    new_thread_lldb = None
                    for t2 in process:
                        if isStoppedForBreakpoint(t2, thread_bp):
                            if t.GetThreadID() in ignore_set:
                                #debug_print("Ignoring a thread that already hit thread_bp (this should never happen)")
                                continue
                            new_thread_lldb = t2
                            break
                    if new_thread_lldb is None:
                        debug_print("Couldn't locate new thread")
                        sys.exit(1)
                    
                    for other_thread in getThreads():
                        if other_thread != new_thread_lldb:
                            if verbose:
                                debug_print("Temporarily suspending other thread", other_thread)
                            other_thread.Suspend()
                    thread_man.onCreateThread({'thread': new_thread_lldb, 'csthread_ptr': new_thread_ptr, 'name': new_thread_name})
                    for fr in new_thread_lldb:
                        debug_print(fr)
                    new_thread_lldb.StepOut()
                    while True:
                        if isUserCode(new_thread_lldb):
                            break
                        new_thread_lldb.StepInstruction(False)
                    for t2 in getThreads():
                        t2.Suspend()
                    
                    #debug_print("Resume", main_thread.is_suspended, running_thread, chosen_cthread, chosen_cthread['thread'] == running_thread)
                    running_thread.Resume()
                    process.Continue()
                    #debug_print("Finished resume", main_thread.GetFrameAtIndex(0))
                    #debug_print(main_thread.stop_reason, main_thread.stop_reason == lldb.eStopReasonBreakpoint)
                    #for fr in main_thread:
                    #    debug_print("\t", fr)
                    handledBreakpoint = True
                    continue
            handlingBreakpoints = handledBreakpoint
        # Send the program state to the visualizer
        if chosen_cthread is not None:
            frame = running_thread.GetFrameAtIndex(0)
            thread_list = []
            for thread in thread_man.getManagedThreads():
                thread_state = thread['state']
                if thread == chosen_cthread:
                    thread_state = 'running'
                locals = None
                if thread_state != 'completed':
                    locals = []
                    frame = thread['thread'].GetFrameAtIndex(0)
                    for local in frame.GetVariables(True, True, False, True):
                        locals.append(serializeVariable(thread_man, local))
                thread_list.append({'name': thread['name'], 'state': thread_state, 'locals': locals})
            globals = frame.get_statics()
            globals_list = []
            for frame_var in globals:
                globals_list.append(serializeVariable(thread_man, frame_var))
            respondToVisualizer({'type': 'res', 'threads': thread_list, 'globals': globals_list})

def isStoppedForBreakpoint(thread, bp):
    return thread.stop_reason == lldb.eStopReasonBreakpoint and thread.GetStopReasonDataAtIndex(0) == bp.GetID()
