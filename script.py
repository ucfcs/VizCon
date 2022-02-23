from time import sleep
from thread_manager import ThreadManager
import lldb
import json
import os
import sys

if len(sys.argv) < 2:
    print("Invalid usage", file=sys.stderr)
    sys.exit(1)
exe = sys.argv[1]
visualizerMode = True
if (len(sys.argv) >= 3 and sys.argv[2] == 'no_visualizer'):
    visualizerMode = False

# TODO
def debug_print(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def waitForVisualizer():
    if not visualizerMode:
        return
    lineIn = sys.stdin.readline()
    objIn = json.loads(lineIn)
def respondToVisualizer(msg):
    sys.stdout.write(json.dumps(msg))
    sys.stdout.write("\n")
    sys.stdout.flush()
def isUserCode(thread):
    frame = thread.GetFrameAtIndex(0)
    line = frame.GetLineEntry()
    if not line.IsValid():
        return False

    if target.GetExecutable() != frame.GetModule().GetFileSpec():
        return False
    file = line.GetFileSpec().GetFilename()
    # TODO: keep this updated
    if "lldb_lib.c" in file:
        return False
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
interleave_log = open("interleave_log.txt", "w")

verbose = False

if not target:
    debug_print("Error creating target")
    sys.exit(2)

# If the target is valid set a breakpoint at main
main_bp = target.BreakpointCreateByName ("real_main", target.GetExecutable().GetFilename())
thread_bp = target.BreakpointCreateByName ("do_post", target.GetExecutable().GetFilename())
vcJoin_bp = target.BreakpointCreateByName ("vcJoin", target.GetExecutable().GetFilename())
vc_internal_registerSem_bp = target.BreakpointCreateByName ("vc_internal_registerSem", target.GetExecutable().GetFilename())
printf_hook_bp = target.BreakpointCreateByName ("printf_hook", target.GetExecutable().GetFilename())

vcWait_bp = target.BreakpointCreateByName ("vcWait", target.GetExecutable().GetFilename())
vcSignal_bp = target.BreakpointCreateByName ("vcSignal", target.GetExecutable().GetFilename())

# Launch the process. Since we specified synchronous mode, we won't return
# from this function until we hit the breakpoint at main
process = target.LaunchSimple (None, None, os.getcwd())


for t in process:
    if t.stop_reason == lldb.eStopReasonBreakpoint and t.GetStopReasonDataAtIndex(0) == main_bp.GetID():
        debug_print("Main thread:", t)
        # TODO: probably get an ID for the thread
        main_thread = t

thread_man = ThreadManager(main_thread)

running_thread = main_thread
chosen_cthread = None
def getThreads():
    for thread in thread_man.getManagedThreads():
        yield thread['thread']
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
    interleave_log.write(chosen_cthread['pthread_id']+"\n")
    for t in getThreads():
        t.Suspend()

    running_thread.Resume()
    if running_thread.GetThreadID() in ignore_set:
        ignore_set.remove(running_thread.GetThreadID())

    running_thread.StepInstruction(False)
    current_function = running_thread.GetFrameAtIndex(0).GetFunction().GetDisplayName()
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
        interleave_log.write("End.\n")
        interleave_log.close()
        respondToVisualizer({'type': 'process_end'})
        sys.exit(0)
    else:
        if not isUserCode(running_thread):
            debug_print("Special stepout from", running_thread.GetFrameAtIndex(0))
            running_thread.StepOut()
    printed_lines = []
    for t in process:
        if t.stop_reason == lldb.eStopReasonBreakpoint and t.GetStopReasonDataAtIndex(0) == printf_hook_bp.GetID(): 
            the_str = t.GetFrameAtIndex(0).FindVariable("str")
            captured_str = process.ReadCStringFromMemory(the_str.GetValueAsUnsigned(), 1024, lldb.SBError())
            printed_lines.append(captured_str)
            process.Continue()
            continue
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
                    debug_print("Ignoring a thread")
                continue
            thread_val = t.GetFrameAtIndex(0).FindVariable("thread").GetValue()
            ignore_set.add(t.GetThreadID())
            thread_man.onJoin(t, thread_val)
        if t.stop_reason == lldb.eStopReasonBreakpoint and t.GetStopReasonDataAtIndex(0) == thread_bp.GetID():
            if t.GetThreadID() in ignore_set:
                if verbose:
                    debug_print("Ignoring a thread that already hit thread_bp")
                continue
            pthread_id = t.GetFrameAtIndex(0).FindVariable("thread").GetValue()
            for other_thread in getThreads():
                if other_thread != t:
                    if verbose:
                        debug_print("Temporarily suspending other thread", other_thread)
                    other_thread.Suspend()
            thread_man.onCreateThread({'thread': t, 'pthread_id': pthread_id})
            t.StepOut()
            while True:
                if isUserCode(t):
                    break
                t.StepInstruction(False)
            for t2 in getThreads():
                t2.Suspend()
            running_thread.Resume()
            process.Continue()
            ignore_set.add(t.GetThreadID())
    # Send the program state to the visualizer
    if chosen_cthread is not None:
        frame = running_thread.GetFrameAtIndex(0)
        thread_list = []
        for thread in thread_man.getManagedThreads():
            thread_state = thread['state']
            if thread == chosen_cthread:
                thread_state = 'running'
            thread_list.append({'name': thread['name'], 'state': thread_state})
        globals = frame.get_statics()
        globals_list = []
        for frame_var in globals:
            global_value = frame_var.GetValue()
            if frame_var.GetTypeName() == 'vcSem *':
                global_value = thread_man.getSemaphoreValue(str(global_value))    
            globals_list.append({'name': frame_var.GetName(), 'type': frame_var.GetTypeName(), 'value': global_value})
        respondToVisualizer({'type': 'res', 'threads': thread_list, 'globals': globals_list, 'printed_lines': printed_lines})