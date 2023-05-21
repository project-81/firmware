"""
A module for project-specific yambs tasks.
"""

# built-in
from pathlib import Path

# third-party
from vcorelib.task import Inbox, Outbox
from vcorelib.task.manager import TaskManager
from vcorelib.task.subprocess.run import SubprocessLogMixin


class GenerateNinja(SubprocessLogMixin):
    """A task for generating ninja configurations."""

    default_requirements = {"vmklib.init", "venv"}

    async def run(self, inbox: Inbox, outbox: Outbox, *args, **kwargs) -> bool:
        """Generate ninja configuration files."""

        params = ["gen"]
        if kwargs.get("watch", False):
            params.append("-w")

        return await self.exec(
            str(inbox["venv"]["venv{python_version}"]["bin"].joinpath("mbs")),
            "-C",
            str(args[0]),
            *params,
        )


def register_yambs(manager: TaskManager, root: Path) -> bool:
    """Register crosstool tasks."""

    manager.register(GenerateNinja("g", root), ["deps"])
    manager.register(GenerateNinja("gw", root, watch=True), ["deps"])
    return True
