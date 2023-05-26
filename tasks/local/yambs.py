"""
A module for project-specific yambs tasks.
"""

# built-in
from pathlib import Path

# third-party
from vcorelib.task import Inbox, Outbox
from vcorelib.task.manager import TaskManager
from vcorelib.task.subprocess.run import SubprocessLogMixin

# internal
from local.common import program_str


class GenerateNinja(SubprocessLogMixin):
    """A task for generating ninja configurations."""

    default_requirements = {"vmklib.init", "venv"}

    async def run(self, inbox: Inbox, outbox: Outbox, *args, **kwargs) -> bool:
        """Generate ninja configuration files."""

        root: Path = args[0]

        early = False

        if (
            kwargs.get("once", False)
            and root.joinpath("build.ninja").is_file()
        ):
            early = True

        result = True

        if kwargs.get("build", False):
            result = await self.exec(program_str("ninja"))

        if early:
            return result

        params = ["gen"]
        if kwargs.get("watch", False):
            params.append("-w")

        return await self.exec(
            str(inbox["venv"]["venv{python_version}"]["bin"].joinpath("mbs")),
            "-C",
            str(root),
            *params,
        )


def register_yambs(manager: TaskManager, root: Path) -> bool:
    """Register crosstool tasks."""

    deps = ["deps"]

    manager.register(GenerateNinja("g", root), deps)
    manager.register(GenerateNinja("go", root, once=True), deps)
    manager.register(GenerateNinja("gb", root, once=True, build=True), deps)
    manager.register(GenerateNinja("gw", root, watch=True), deps)

    return True
