from time import sleep
from utils import VizconErrorCode
from thread_manager import ThreadManager
import lldb
import ctypes
import json
import base64
import os
import sys
import traceback

def start(exe, terminalOutputFile, visualizerMode):
    try:
        if terminalOutputFile is not None:
            terminalOutputFile = base64.b64decode(terminalOutputFile).decode()
        _start(base64.b64decode(exe).decode(), terminalOutputFile, visualizerMode)
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
def debug_now(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)
    sys.stderr.flush()
    sys.stdout.flush()
def debug_print(*args, **kwargs):
    return
    #print(*args, file=sys.stderr, **kwargs)
    #sys.stderr.flush()
    #sys.stdout.flush()

def do_nothing():
    pass
def makeSuspendFixer(process):
    if os.name != "nt":
        return do_nothing
    HANDLE = ctypes.c_voidp
    DWORD = ctypes.c_ulong
    BOOL = ctypes.c_int
    SuspendThread = ctypes.windll.kernel32.SuspendThread
    SuspendThread.argtypes = (HANDLE,)
    SuspendThread.restype = DWORD
    ResumeThread = ctypes.windll.kernel32.ResumeThread
    ResumeThread.argtypes = (HANDLE,)
    ResumeThread.restype = DWORD
    OpenThread = ctypes.windll.kernel32.OpenThread
    OpenThread.argtypes = (DWORD, BOOL, DWORD)
    OpenThread.restype = HANDLE
    THREAD_SUSPEND_RESUME = 2
    CloseHandle = ctypes.windll.kernel32.CloseHandle
    CloseHandle.argtypes = (HANDLE,)
    CloseHandle.restype = BOOL
    def suspendFixer():
        for t in process:
            tid = t.GetThreadID()
            thread_handle = OpenThread(THREAD_SUSPEND_RESUME, 0, tid)
            if thread_handle == 0:
                debug_print("Error. Couldn't open thread handle!")
                continue
            prev_count = SuspendThread(thread_handle)
            count = prev_count + 1
            if prev_count > 127:
                debug_print("Suspend error.")
                prev_count = ResumeThread(thread_handle)
                count = prev_count - 1
                if prev_count > 127:
                    debug_print("Failed to fix suspend bug on thread", t)
                    CloseHandle(thread_handle)
                    continue
            target_val = 1 if prev_count > 0 else 0
            if target_val == 0:
                debug_print("Surprisingly, target value is 0")
            original_count = count
            while count > target_val:
                prev_count = ResumeThread(thread_handle)
                if prev_count > 127:
                    debug_print("Error. Failed to fix suspend bug on thread")
                    break
                count = prev_count - 1
            #debug_now("Final count", count, "target was", target_val, "original was", original_count, t )
            CloseHandle(thread_handle)
    return suspendFixer
def _start(exe, terminalOutputFile, visualizerMode):
    if not visualizerMode:
        dataOutputFile = sys.stdout
    else:
        dataOutputFile = os.fdopen(3, 'w')

    def waitForVisualizer():
        if not visualizerMode:
            return
        lineIn = sys.stdin.readline()
        objIn = json.loads(lineIn)
    def respondToVisualizer(msg):
        dataOutputFile.write(json.dumps(msg))
        dataOutputFile.write("\n")
        dataOutputFile.flush()
    def reportError(error):
        err_code = error.value
        respondToVisualizer({'type': 'vc_error', 'errCode': err_code})
        # This shouldn't ever happen but check just in case there's a bug in the future.
        # Make sure we never exit with code 0 because that would be hard to debug.
        if err_code == 0:
            debug_now("reportError called with exit code 0")
            sys.exit(10)
        sys.exit(err_code * -1)
    def isUserCode(thread):
        frame = thread.GetFrameAtIndex(0)
        return isFrameUserCode(frame)
    def isFrameUserCode(frame):
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
        if "semaphores.c" in file:
            return False
        if "mutexes.c" in file:
            return False
        if "utils.c" in file:
            return False
        if "threads.c" in file:
            return False
        full_file = str(line.GetFileSpec())
        if "platform/zig" in full_file or "platform\zig" in full_file:
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
        respondToVisualizer({'type': 'start_error', 'error': 'Visualizer launch error: Failed to create target process'})
        sys.exit(2)

    def createBreakpoint(name):
        bp = target.BreakpointCreateByName(name, target.GetExecutable().GetFilename())
        assert bp
        assert bp.IsValid()
        assert bp.IsEnabled()
        assert bp.GetNumLocations() == 1
        return bp
    # If the target is valid set a breakpoint at main
    main_bp = createBreakpoint("userMain")
    thread_bp = createBreakpoint("do_post")
    hook_createThread_bp = createBreakpoint("lldb_hook_createThread")
    vcJoin_bp = createBreakpoint("lldb_hook_joinThread")
    hook_freeThread_bp = createBreakpoint("lldb_hook_freeThread")
    hook_threadSleep_bp = createBreakpoint("lldb_hook_threadSleep")

    # Semaphore breakpoints
    vc_internal_registerSem_bp = createBreakpoint("lldb_hook_registerSem")
    vcWait_bp = createBreakpoint("lldb_hook_semWait")
    vcSignal_bp = createBreakpoint("lldb_hook_semSignal")
    hook_semTryWait_bp = createBreakpoint("lldb_hook_semTryWait")
    hook_semClose_bp = createBreakpoint("lldb_hook_semClose")

    # Mutex breakpoints
    registerMutex_bp = createBreakpoint("lldb_hook_registerMutex")
    lockMutex_bp = createBreakpoint("lldb_hook_lockMutex")
    unlockMutex_bp = createBreakpoint("lldb_hook_unlockMutex")
    hook_mutexTryLock_bp = createBreakpoint("lldb_hook_mutexTryLock")
    hook_mutexClose_bp = createBreakpoint("lldb_hook_mutexClose")

    launch_info = target.GetLaunchInfo()

    error = lldb.SBError()
    #print(launch_info.AddOpenFileAction(0, "CONIN$", True, False))
    if terminalOutputFile is not None:
        launch_info.AddOpenFileAction(1, terminalOutputFile, False, True)
        launch_info.AddOpenFileAction(2, terminalOutputFile, False, True)
    launch_info.SetEnvironmentEntries(["lldbMode=1"], True)
    process = target.Launch(launch_info, error)
    debug_print("Launch info:", error.Success(), error, error.GetCString())

    main_thread = None
    for t in process:
        if isStoppedForBreakpoint(t, main_bp):
            debug_print("Main thread:", t)
            # TODO: probably get an ID for the thread
            main_thread = t
    if main_thread is None:
        respondToVisualizer({'type': 'start_error', 'error': 'Visualizer launch error: Failed to find main thread'})
        sys.exit(20)
    thread_man = ThreadManager(main_thread)
    # LLDB on Windows has a bug where it reaches MAXIMUM_SUSPEND_COUNT after calling SuspendThread too many times.
    # Periodically making calls to fixSuspendBug serves as a quick hack to hide the issue by draining the counter
    # On non-Windows platforms, fixSuspendBug is a no-op (the do_nothing function)
    fixSuspendBug = makeSuspendFixer(process)
    running_thread = main_thread
    chosen_cthread = None
    def getThreads():
        for thread in thread_man.getManagedThreads():
            yield thread['thread']
    def serializeVariable(thread_man, lldb_var):
        var_value = lldb_var.GetValue()
        type_name = lldb_var.GetTypeName()
        if type_name == 'CSSem *':
            var_value = thread_man.getSemaphoreValue(str(var_value))
            type_name = "vcSem"
        elif type_name == 'CSMutex *':
            owner = thread_man.getMutexOwner(str(var_value))
            type_name = "vcMutex"
            if owner is None:
                var_value = "Unlocked"
            else:
                var_value = "Locked by " + owner['name']
        else:
            out = lldb.SBStream()
            lldb_var.GetDescription(out)
            desc = out.GetData()
            loc = desc.index(" = ")
            var_value = desc[loc + len(" = "):]
        return {'name': lldb_var.GetName(), 'type': type_name, 'value': var_value}
    def returnIntFromFrame(thread, frame, int_val):
        # CreateValueFromData is an instance method, so we need a valid SBValue to start with.
        # There's probably another way to do this, but this is easy
        lldbActiveValue = thread.GetFrameAtIndex(0).GetModule().FindFirstGlobalVariable(target, "isLldbActive")
        assert lldbActiveValue.IsValid()
        
        result_sbdata = lldb.SBData.CreateDataFromSInt32Array(lldb.eByteOrderLittle, 8, [int_val])
        result_sbvalue = lldbActiveValue.CreateValueFromData("retval", result_sbdata, target.GetBasicType(lldb.eBasicTypeInt))
        thread.ReturnFromFrame(frame, result_sbvalue)

    ignore_set = set()
    respondToVisualizer({'type': 'hello'})
    while True:
        waitForVisualizer()
        if process.state == lldb.eStateExited:
            respondToVisualizer({'type': 'process_end', 'code': process.GetExitStatus()})
            sys.exit(0)
            break
        fixSuspendBug()
        chosen_cthread = thread_man.chooseThread()

        if chosen_cthread is None:
            debug_print("Ready list is empty! No more runnable threads! Deadlock?")
            reportError(VizconErrorCode.VC_ERROR_DEADLOCK)
        running_thread = chosen_cthread['thread']
        #debug_print("Selected", chosen_cthread['name'], running_thread.GetFrameAtIndex(0), running_thread, running_thread.GetStopReason(), running_thread.GetStopDescription(1024))
        for t in getThreads():
            t.Suspend()

        running_thread.Resume()
        if running_thread.GetThreadID() in ignore_set:
            ignore_set.remove(running_thread.GetThreadID())

        running_thread.StepInstruction(False)
        current_function = running_thread.GetFrameAtIndex(0).GetFunction().GetDisplayName()
        #debug_print("Step one instruction", current_function, running_thread.GetFrameAtIndex(0).IsValid(), running_thread.GetFrameAtIndex(0), running_thread, running_thread.GetStopReason(), running_thread.GetStopDescription(1024))
        if current_function == 'vc_internal_thread_wrapper':
            debug_print("Thread exit")
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
            running_thread.StepOut()
            # TODO: it might be appropriate to resume all the threads that were waiting
            # but it probably doesn't matter
        elif current_function == 'main':
            # TODO: what if other threads are still running?
            debug_print("Main function exited.")
            # Allow the process to finish (allows cleanup to run)
            for t in process:
                t.Resume()
            process.Continue()
            while process.state != lldb.eStateExited:
                process.Continue()
            respondToVisualizer({'type': 'process_end', 'code': process.GetExitStatus()})
            sys.exit(0)
        else:
            if not isUserCode(running_thread):
                last_nonuser_code_fr = None
                for fr in running_thread:
                    if isFrameUserCode(fr):
                        break
                    last_nonuser_code_fr = fr
                #debug_print(chosen_cthread['name'], " entered function", last_nonuser_code_fr, ". Stepping out.")
                running_thread.StepOutOfFrame(last_nonuser_code_fr)
        handlingBreakpoints = True
        while handlingBreakpoints:
            handledBreakpoint = False
            for t in process:
                # Semaphores:
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
                    t.StepInstruction(False)
                    handledBreakpoint = True
                    continue
                if isStoppedForBreakpoint(t, vcSignal_bp):
                    new_sem = t.GetFrameAtIndex(0).FindVariable("sem").GetValue()
                    vc_res = thread_man.onSignalSem(t, str(new_sem))
                    returnIntFromFrame(t, t.GetFrameAtIndex(0), vc_res.value)
                    t.StepOut()
                    handledBreakpoint = True
                    continue
                if isStoppedForBreakpoint(t, hook_semTryWait_bp):
                    sem = t.GetFrameAtIndex(0).FindVariable("sem").GetValue()
                    result = thread_man.onTryWaitSem(t, str(sem))
                    returnIntFromFrame(t, t.GetFrameAtIndex(0), 1 if result else 0)
                    t.StepOut()
                    handledBreakpoint = True
                    continue
                if isStoppedForBreakpoint(t, hook_semClose_bp):
                    new_sem = t.GetFrameAtIndex(0).FindVariable("sem").GetValue()
                    thread_man.onCloseSem(t, str(new_sem))
                    process.Continue()
                    handledBreakpoint = True
                    continue
                # Mutexes:
                if isStoppedForBreakpoint(t, registerMutex_bp):
                    new_mutex = t.GetFrameAtIndex(0).FindVariable("mutex").GetValue()
                    thread_man.registerMutex(str(new_mutex))
                    process.Continue()
                    handledBreakpoint = True
                    continue
                if isStoppedForBreakpoint(t, lockMutex_bp):
                    mutex_ptr = t.GetFrameAtIndex(0).FindVariable("mutex").GetValue()
                    vc_res = thread_man.onLockMutex(t, str(mutex_ptr))
                    returnIntFromFrame(t, t.GetFrameAtIndex(0), vc_res.value)
                    t.StepInstruction(False) # returnFromFrame doesn't reset the stop reason
                    handledBreakpoint = True
                    continue
                if isStoppedForBreakpoint(t, unlockMutex_bp):
                    mutex_ptr = t.GetFrameAtIndex(0).FindVariable("mutex").GetValue()
                    vc_res = thread_man.onUnlockMutex(t, str(mutex_ptr))
                    returnIntFromFrame(t, t.GetFrameAtIndex(0), vc_res.value)
                    t.StepOut()
                    handledBreakpoint = True
                    continue
                if isStoppedForBreakpoint(t, hook_mutexTryLock_bp):
                    mutex = t.GetFrameAtIndex(0).FindVariable("mutex").GetValue()
                    result = thread_man.onTryLockMutex(t, str(mutex))
                    returnIntFromFrame(t, t.GetFrameAtIndex(0), 1 if result else 0)
                    t.StepOut()
                    handledBreakpoint = True
                    continue
                if isStoppedForBreakpoint(t, hook_mutexClose_bp):
                    mutex_ptr = t.GetFrameAtIndex(0).FindVariable("mutex").GetValue()
                    thread_man.onCloseMutex(t, str(mutex_ptr))
                    process.Continue()
                    handledBreakpoint = True
                    continue
                # Other:
                if isStoppedForBreakpoint(t, vcJoin_bp):
                    if t.GetThreadID() in ignore_set:
                        if verbose:
                            debug_print("Ignoring a thread")
                        continue
                    thread_val = t.GetFrameAtIndex(0).FindVariable("thread").GetValue()
                    ignore_set.add(t.GetThreadID())
                    thread_man.onJoin(t, thread_val)
                    t.StepInstruction(False)
                    continue
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
                        sys.exit(10)
                    
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
                if isStoppedForBreakpoint(t, hook_freeThread_bp):
                    thread_ptr = t.GetFrameAtIndex(0).FindVariable("thread").GetValue()
                    thread_man.onFreeThread(t, thread_ptr)
                    process.Continue()
                    handledBreakpoint = True
                    continue
                if isStoppedForBreakpoint(t, hook_threadSleep_bp):
                    milliseconds = t.GetFrameAtIndex(0).FindVariable("milliseconds").GetValueAsSigned()
                    thread_man.onSleepThread(t, milliseconds)
                    t.StepInstruction(False)
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
                    for frame in thread['thread']:
                        if isFrameUserCode(frame):
                            break
                    #frame = thread['thread'].GetFrameAtIndex(0)
                    for local in frame.GetVariables(True, True, False, True):
                        locals.append(serializeVariable(thread_man, local))
                thread_list.append({'name': thread['name'], 'state': thread_state, 'locals': locals})
            frame = None
            for frame in main_thread:
                if isFrameUserCode(frame):
                    break
            globals = frame.get_statics()
            globals_list = []
            for frame_var in globals:
                globals_list.append(serializeVariable(thread_man, frame_var))
            respondToVisualizer({'type': 'res', 'threads': thread_list, 'globals': globals_list})
        #debug_print(chosen_cthread['name'], "completed stepping (double). Ended at", running_thread.GetFrameAtIndex(0))

def isStoppedForBreakpoint(thread, bp):
    return thread.stop_reason == lldb.eStopReasonBreakpoint and thread.GetStopReasonDataAtIndex(0) == bp.GetID()
