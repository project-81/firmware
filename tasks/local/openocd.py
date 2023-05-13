"""
A module for project-specific openocd tasks.
"""

# built-in
from pathlib import Path

# third-party
from vcorelib.task import Inbox, Outbox
from vcorelib.task.manager import TaskManager
from vcorelib.task.subprocess.run import SubprocessLogMixin

# internal
from local.common import add_program_path, program_str
from local.configure import ThirdPartyConfigure


class OpenocdTask(SubprocessLogMixin):
    """A class for running openocd and connecting to a target."""

    default_requirements = {"vmklib.init", "build-openocd"}

    async def run(self, inbox: Inbox, outbox: Outbox, *args, **kwargs) -> bool:
        """Editable-install a third-party dependency."""

        return await self.exec(
            program_str("openocd"),
            "-s",
            str(args[0].joinpath("tcl")),
            "-f",
            f"interface/{kwargs['interface']}.cfg",
        )


def register_openocd(manager: TaskManager, third_party: Path) -> bool:
    """Register openocd targets."""

    proj = "openocd"

    # Build openocd.
    manager.register(
        ThirdPartyConfigure(
            f"build-{proj}",
            third_party,
            project=proj,
            make=True,
        ),
        ["github-clone.openocd-org.openocd"],
    )

    repo = third_party.joinpath(proj)

    add_program_path(proj, repo, "src", proj)

    manager.register(OpenocdTask(proj + "-{interface}", repo), [])

    return True
