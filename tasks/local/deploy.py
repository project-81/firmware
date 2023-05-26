"""
A module for deploying applications to devices.
"""

# third-party
from vcorelib.task import Inbox, Outbox
from vcorelib.task.manager import TaskManager
from yambs.config import Config

# internal
from local.mixin import BoardAppMixin


class Deploy(BoardAppMixin):
    """A task implementation for deploying firmware."""

    async def run(self, inbox: Inbox, outbox: Outbox, *args, **kwargs) -> bool:
        """Deploy new firmware."""

        config: Config = args[0]

        result = await self.select_board_app(config, **kwargs)
        if result is None:
            return False

        # Always attempt to build.
        assert await self.exec("ninja")

        # Perform the update.
        if "pico" in result[0] or "xiao" in result[0]:
            assert await self.exec(
                "picotool", "load", str(result[1].with_suffix(".uf2"))
            )
            assert await self.exec("picotool", "reboot")

        return True


def register_deploy(manager: TaskManager, config: Config) -> bool:
    """Register deploy tasks."""

    manager.register(Deploy("d", config), ["deps"])
    return True
