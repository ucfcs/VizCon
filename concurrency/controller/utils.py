from enum import Enum, unique

@unique
class VizconErrorCode(Enum):
    VC_ERROR_DOUBLELOCK = -12
    VC_ERROR_CROSSTHREADUNLOCK = -13
    VC_ERROR_SEMVALUELIMIT = -14
    # This error only exists in the visualizer.
    VC_ERROR_DEADLOCK = 15

class SemValueLimitError(Exception):
    pass

class DoubleLockError(Exception):
    pass

class CrossThreadUnlockError(Exception):
    pass