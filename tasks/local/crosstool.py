"""
A module for project-specific crosstool-ng tasks.
"""

# built-in
from pathlib import Path

# third-party
from vcorelib.task import Inbox, Outbox, Phony
from vcorelib.task.manager import TaskManager
from vcorelib.task.subprocess.run import SubprocessLogMixin
from vmklib.tasks.mixins.concrete import ConcreteOnceMixin

# internal
from local.common import add_path


class CrosstoolTask(ConcreteOnceMixin, SubprocessLogMixin):
    """
    A class implementing a task for building toolchains with crosstool-ng.
    """

    async def run(self, inbox: Inbox, outbox: Outbox, *args, **kwargs) -> bool:
        """Build a specific toolchain."""

        cwd = args[0].joinpath(kwargs["toolchain"])
        return await self.exec("ct-ng", "-C", str(cwd), "build")


def register_crosstool(manager: TaskManager, root: Path) -> bool:
    """Register crosstool tasks."""

    toolchains_root = root.joinpath("toolchains")

    manager.register(
        CrosstoolTask("crosstool-{toolchain}", toolchains_root),
        ["github-clone.picolibc.picolibc"],
    )

    # Add toolchains to path.
    # , "arm-none-eabi"
    toolchains = ["arm-picolibc-eabi"]
    for toolchain in toolchains:
        add_path(toolchains_root.joinpath(toolchain, "out", "bin"))
        root.joinpath("build", toolchain).mkdir(parents=True, exist_ok=True)

    manager.register(
        Phony("toolchains"), [f"crosstool-{x}" for x in toolchains]
    )

    return True
