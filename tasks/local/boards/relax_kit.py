"""
A module for interacting with the XMC4700 Relax Kit.
https://www.infineon.com/cms/en/product/evaluation-boards/kit_xmc47_relax_v1/
"""

# built-in
from pathlib import Path

# internal
from local.jlink import JlinkTask

# third-party
from vcorelib.task.manager import TaskManager

BOARD_ARGS = {
    "relax_kit": ["-device", "XMC4700-2048", "-if", "SWD", "-speed", "auto"]
}

PORTS = {"gdb": 2331, "swo": 2332, "telnet": 2333, "rtt": 19021}

COMMON_ARGS = [
    "-port",
    str(PORTS["gdb"]),
    "-swoport",
    str(PORTS["swo"]),
    "-telnetport",
    str(PORTS["telnet"]),
    "-RTTTelnetPort",
    str(PORTS["rtt"]),
    "-endian",
    "little",
    "-nogui",
    "-strict",
]


def jlink_gdbserver_task(
    board: str, third_party: Path, name: str = None
) -> JlinkTask:
    """Create a JLink GDB server task."""

    if name is None:
        name = board

    return JlinkTask(
        name,
        third_party,
        *BOARD_ARGS[board],
        *COMMON_ARGS,
        program="JLinkGDBServer"
    )


def register_relax_kit(manager: TaskManager, third_party: Path) -> bool:
    """Register SEGGER JLink-related tasks."""

    manager.register(jlink_gdbserver_task("relax_kit", third_party), [])
    return True
