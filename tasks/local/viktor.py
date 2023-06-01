"""
A module for running viktor.
"""

# built-in
from pathlib import Path

# third-party
from vcorelib.io import ARBITER
from vcorelib.paths.context import tempfile
from vcorelib.task import Inbox, Outbox
from vcorelib.task.manager import TaskManager
from yambs.config import Config

# internal
from local.mixin import BoardAppMixin
from local.prompts import manual_select

BY_ID = Path("/dev/serial/by-id")


class RunViktor(BoardAppMixin):
    """A task implementation for running viktor."""

    async def run(self, inbox: Inbox, outbox: Outbox, *args, **kwargs) -> bool:
        """Run viktor."""

        config: Config = args[0]

        board = kwargs.get("board", "native")

        result = await self.select_board_app(
            config,
            board=board,
            app="viktor",
            deploy_json=False,
        )
        if result is None:
            return False

        entry = result[1].with_suffix(".elf")

        # Select a serial port. At some point we may want to allow prompting
        # for multiple serial ports (if there's more than one).
        port = manual_select("tty", (str(x) for x in BY_ID.iterdir()))
        if port is None:
            return False

        # At some point we should meld this with a file (or - instead of that,
        # schema validation should provide defaults).
        config_data = {
            "ttys": [str(Path(port).resolve())],
            "port": kwargs.get("port", 0),
        }

        # Create the configuration data.
        with tempfile(suffix=".json") as json_config:
            ARBITER.encode(json_config, config_data)  # type: ignore
            result = await self.exec(str(entry), str(json_config))

        return result


def register_viktor(manager: TaskManager, config: Config) -> bool:
    """Register native tasks."""

    manager.register(RunViktor("v", config), ["gb"])
    return True
