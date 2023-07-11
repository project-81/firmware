"""
A module for working with Saleae Logic 2.
"""

# built-in
from pathlib import Path

# third-party
from vcorelib.paths import set_exec_flags
from vcorelib.task import Inbox, Outbox
from vcorelib.task.manager import TaskManager

# internal
from local.mixin import ShellCmdInDirMixin

LOGIC_VERSION = "2.4.8"
LOGIC_IMAGE = f"Logic-{LOGIC_VERSION}-master.AppImage"


class Logic(ShellCmdInDirMixin):
    """A class for extracting Logic software."""

    async def run(self, inbox: Inbox, outbox: Outbox, *args, **kwargs) -> bool:
        """Run the task."""

        third_party = args[0]

        result = True

        image = third_party.joinpath("tarballs", "logic", LOGIC_IMAGE)
        if not image.is_file():
            image.parent.mkdir(exist_ok=True)

            result &= await self.exec(
                "wget",
                "https://logic2api.saleae.com/download?os=linux",
                "-O",
                str(image),
            )
            if result:
                set_exec_flags(image)

        squashfs = image.parent.joinpath("squashfs-root")

        if not squashfs.is_dir():
            result &= await self.shell_cmd_in_dir(
                image.parent, [str(image), "--appimage-extract"]
            )

        return await self.exec(str(squashfs.joinpath("Logic")))


def register_logic(manager: TaskManager, third_party: Path) -> bool:
    """Register Saleae Logic-related tasks."""

    manager.register(Logic("logic", third_party), [])

    return True
