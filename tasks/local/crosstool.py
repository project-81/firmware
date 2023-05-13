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


class CrosstoolTask(ConcreteOnceMixin, SubprocessLogMixin):
    """
    A class implementing a task for building toolchains with crosstool-ng.
    """

    default_requirements = {"vmklib.init", "third-party-clones"}

    async def run(self, inbox: Inbox, outbox: Outbox, *args, **kwargs) -> bool:
        """Editable-install a third-party dependency."""

        cwd = args[0].joinpath("toolchains", kwargs["toolchain"])
        return await self.exec("ct-ng", "-C", str(cwd), "build")


def register_crosstool(manager: TaskManager, third_party: Path) -> bool:
    """Register python tasks."""

    manager.register(CrosstoolTask("crosstool-{toolchain}", third_party), [])

    # Add toolchains to path.
    toolchains = ["arm-picolibc-eabi"]

    manager.register(
        Phony("toolchains"), [f"crosstool-{x}" for x in toolchains]
    )
