from time import sleep
from thread_manager import ThreadManager
import lldb
import json
import os
import sys

# Set the path to the executable to debug
exe = "race-test.exe"

# TODO
ogprint = print
def print(*args, **kwargs):
    ogprint(*args, file=sys.stderr, **kwargs)

def waitForVisualizer():
    lineIn = sys.stdin.readline()
    objIn = json.loads(lineIn)
def respondToVisualizer(msg):
    sys.stdout.write(json.dumps(msg))
    sys.stdout.write("\n")
    sys.stdout.flush()

os.environ['LLDB_LAUNCH_INFERIORS_WITHOUT_CONSOLE'] = str(True)
# Create a new debugger instance
debugger = lldb.SBDebugger.Create()

# When we step or continue, don't return from the function until the process 
# stops. Otherwise we would have to handle the process events ourselves which, while doable is
#a little tricky.  We do this by setting the async mode to false.
debugger.SetAsync(False)

# Create a target from a file and arch
print("Creating a target for " + exe)

#target = debugger.CreateTargetWithFileAndArch(exe, lldb.LLDB_ARCH_DEFAULT)
target = debugger.CreateTarget(exe)
interleave_log = open("interleave_log.txt", "w")

# verbose: print("Target", target)
verbose = False

if target:
    # If the target is valid set a breakpoint at main
    main_bp = target.BreakpointCreateByName ("real_main", target.GetExecutable().GetFilename())
    thread_bp = target.BreakpointCreateByName ("do_post", target.GetExecutable().GetFilename())
    vcJoin_bp = target.BreakpointCreateByName ("vcJoin", target.GetExecutable().GetFilename())
    vc_internal_registerSem_bp = target.BreakpointCreateByName ("vc_internal_registerSem", target.GetExecutable().GetFilename())
    vcWait_bp = target.BreakpointCreateByName ("vcWait", target.GetExecutable().GetFilename())
    vcSignal_bp = target.BreakpointCreateByName ("vcSignal", target.GetExecutable().GetFilename())

    # TODO: This is kind of cheating. Fixme
    user_thread_bp = target.BreakpointCreateByName ("function", target.GetExecutable().GetFilename())

    # Launch the process. Since we specified synchronous mode, we won't return
    # from this function until we hit the breakpoint at main
    process = target.LaunchSimple (None, None, os.getcwd())
    
    
    for t in process:
        if t.stop_reason == lldb.eStopReasonBreakpoint and t.GetStopReasonDataAtIndex(0) == main_bp.GetID():
            print("Main thread:", t)
            # TODO: probably get an ID for the thread
            main_thread = t

    thread_man = ThreadManager(main_thread)
    main_vars = main_thread.GetFrameAtIndex(0).GetVariables(False, False, True, True)
    print("Searched vars", main_thread.GetFrameAtIndex(0))
    for var in main_vars:
        print("Global", var)
    running_thread = main_thread
    chosen_cthread = None
    def getThreads():
        for thread in thread_man.getManagedThreads():
            yield thread['thread']
    ignore_set = set()
    print("-----------------------------------")
    respondToVisualizer({'type': 'hello'})
    while True:
        for t in process:
            if t.stop_reason == lldb.eStopReasonBreakpoint and t.GetStopReasonDataAtIndex(0) == vc_internal_registerSem_bp.GetID():
                new_sem = t.GetFrameAtIndex(0).FindVariable("sem").GetValue()
                # TODO: why is this a problem when vc_internal_init is stepi instead of stepover
                thread_man.registerSem(str(new_sem))
                process.Continue()
                continue
            if t.stop_reason == lldb.eStopReasonBreakpoint and t.GetStopReasonDataAtIndex(0) == vcWait_bp.GetID():
                new_sem = t.GetFrameAtIndex(0).FindVariable("sem").GetValue()
                # TODO: why is this a problem when vc_internal_init is stepi instead of stepover
                thread_man.onWaitSem(t, str(new_sem))
                # TODO: Add a better resume to replace this
                process.Continue()
                continue
            if t.stop_reason == lldb.eStopReasonBreakpoint and t.GetStopReasonDataAtIndex(0) == vcSignal_bp.GetID():
                new_sem = t.GetFrameAtIndex(0).FindVariable("sem").GetValue()
                # TODO: why is this a problem when vc_internal_init is stepi instead of stepover
                thread_man.onSignalSem(t, str(new_sem))
                process.Continue()
                continue
            if t.stop_reason == lldb.eStopReasonBreakpoint and t.GetStopReasonDataAtIndex(0) == vcJoin_bp.GetID():
                if t.GetThreadID() in ignore_set:
                    if verbose:
                        print("Ignoring a thread")
                    continue
                thread_val = t.GetFrameAtIndex(0).FindVariable("thread").GetValue()
                #verbose: print("You joined on", thread_val)
                
                ignore_set.add(t.GetThreadID())
                thread_man.onJoin(t, thread_val)
            if t.stop_reason == lldb.eStopReasonBreakpoint and t.GetStopReasonDataAtIndex(0) == thread_bp.GetID():
                if t.GetThreadID() in ignore_set:
                    if verbose:
                        print("Ignoring a thread that already hit thread_bp")
                    continue
                #verbose: print(t, "is stopped because of the thread breakpoint")
                pthread_id = t.GetFrameAtIndex(0).FindVariable("thread").GetValue()
                for other_thread in getThreads():
                    if other_thread != t:
                        if verbose:
                            print("Temporarily suspending other thread", other_thread)
                        other_thread.Suspend()
                thread_man.onCreateThread({'thread': t, 'pthread_id': pthread_id})
                process.Continue()
                if t.stop_reason == lldb.eStopReasonBreakpoint and t.GetStopReasonDataAtIndex(0) == user_thread_bp.GetID():
                    if verbose:
                        print("User thread function hit successfully")
                        print(running_thread.stop_reason == lldb.eStopReasonNone, running_thread)
                    for t2 in getThreads():
                        t2.Suspend()
                    running_thread.Resume()
                    process.Continue()
                    ignore_set.add(t.GetThreadID())
                else:
                    print("Unknown stop while trying to hit user thread function!")
        if process.state == lldb.eStateExited:
            print("Process state is", process)
            break
        if chosen_cthread is not None:
            frame = running_thread.GetFrameAtIndex(0)
            #globals = frame.get_statics()
            #arguments = frame.get_arguments()
            #locals = frame.get_locals()
            #print("Variables:")
            #for frame_var in globals:
            #    print("\t(Global)",frame_var)
            #for frame_var in arguments:
            #    print("\t(Argument)",frame_var)
            #for frame_var in locals:
            #    print("\t(Local)",frame_var)
            thread_list = []
            for thread in thread_man.getManagedThreads():
                thread_state = thread['state']
                if thread == chosen_cthread:
                    thread_state = 'running'
                thread_list.append({'name': thread['pthread_id'], 'active': thread == chosen_cthread, 'state': thread_state})
            globals = frame.get_statics()
            globals_list = []
            for frame_var in globals:
                globals_list.append({'name': frame_var.GetName(), 'type': frame_var.GetTypeName(), 'value': frame_var.GetValue()})
            respondToVisualizer({'type': 'res', 'threads': thread_list, 'globals': globals_list})
        waitForVisualizer()
        chosen_cthread = thread_man.chooseThread()
        
        if chosen_cthread is None:
            print("Ready list is empty! No more runnable threads! Deadlock?")
            print(running_thread.GetFrameAtIndex(0).EvaluateExpression("counter"))
            sys.exit(1)
        running_thread = chosen_cthread['thread']
        interleave_log.write(chosen_cthread['pthread_id']+"\n")
        for t in getThreads():
            t.Suspend()
        #for t in process:
        #    print("Thread in process", t.is_suspended, t)
        running_thread.Resume()
        if running_thread.GetThreadID() in ignore_set:
            ignore_set.remove(running_thread.GetThreadID())

        line = running_thread.GetFrameAtIndex(0).GetLineEntry()
        current_function = running_thread.GetFrameAtIndex(0).GetFunction().GetDisplayName()
        if verbose:
            print("Stepping instruction", line, current_function)
        running_thread.StepInstruction(False)
        current_function = running_thread.GetFrameAtIndex(0).GetFunction().GetDisplayName()
        if current_function == 'vcJoin' or current_function == 'vcWait' or current_function == 'vcSignal' or current_function == 'vcCreateSemaphore' or current_function == 'vc_internal_init' or current_function == 'doCreateThread':
            if verbose:
                print("Stepping out of vcJoin")
            running_thread.StepOut()
            if verbose:
                print("Done stepping out. maybe.", running_thread.GetFrameAtIndex(0).GetFunction().GetDisplayName())
                print(running_thread.stop_reason, running_thread.stop_reason ==  lldb.eStopReasonBreakpoint)
        #     print("You are in vcJoin!")
        #     thread_val = running_thread.GetFrameAtIndex(0).FindVariable("thread")
        #     print(thread_val, thread_val.GetValueAsUnsigned())
        elif current_function == 'vc_internal_thread_wrapper':
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
            print("Main function exited.")
            print(running_thread.GetFrameAtIndex(0).EvaluateExpression("counter"))
            interleave_log.write("End.\n")
            interleave_log.close()
            respondToVisualizer({'type': 'process_end'})
            sys.exit(0)
        else:
            line = running_thread.GetFrameAtIndex(0).GetLineEntry()
            isOurs = line.IsValid() and "race-test.c" in line.GetFileSpec().GetFilename()
            if not isOurs:
                #print("Stepi into line", line, running_thread.GetFrameAtIndex(0))
                #print("Stepping out of it")
                running_thread.StepOut()