"""
A module for project-specific task registration.
"""

# built-in
from pathlib import Path
from sys import executable
from typing import Dict

# third-party
from vcorelib.task import Phony
from vcorelib.task.manager import TaskManager
from yambs.config import Config

# internal
from local.boards import register_boards
from local.common import PATHS, add_path
from local.crosstool import register_crosstool
from local.deploy import register_deploy
from local.gdbgui import register_gdbgui
from local.git import register_git
from local.jlink import register_jlink
from local.logic import register_logic
from local.native import register_native
from local.ninja import register_ninja
from local.node import register_node
from local.openocd import register_openocd
from local.python import register_python
from local.viktor import register_viktor
from local.yambs import register_yambs


def register(
    manager: TaskManager,
    project: str,
    cwd: Path,
    substitutions: Dict[str, str],
) -> bool:
    """Register project tasks to the manager."""

    del substitutions

    third_party = cwd.joinpath("third-party")
    third_party.joinpath("tarballs").mkdir(parents=True, exist_ok=True)

    # Ensure scripts in the virtual environment are available on the path.
    add_path(Path(executable).parent)

    # Set some paths here for convenience.
    PATHS["build"] = cwd.joinpath("build")
    PATHS["third-party"] = third_party

    for reg in [
        register_git,
        register_python,
        register_ninja,
        register_node,
        register_openocd,
        register_gdbgui,
        register_jlink,
        register_logic,
        register_boards,
    ]:
        assert reg(manager, third_party), reg

    for cwd_reg in [register_crosstool, register_yambs]:
        assert cwd_reg(manager, cwd), cwd_reg

    config = Config.load()
    for config_reg in [register_deploy, register_native, register_viktor]:
        assert config_reg(manager, config), config_reg

    manager.register(
        Phony("third-party-clones"),
        [
            "github-shallow-clone.Cherrg.gdbgui-fix_447",
            "github-clone.raspberrypi.pico-sdk",
            "github-clone.raspberrypi.pico-examples",
            "github-clone.nlohmann.json",
        ],
    )

    manager.register(Phony("deps"), ["toolchains", "pioasm"])

    # Default target.
    manager.register(Phony("all"), ["gb"])

    del project

    return True
