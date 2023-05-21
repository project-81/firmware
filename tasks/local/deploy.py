"""
A module for deploying applications to devices.
"""

# built-in
from pathlib import Path
from typing import Dict, Optional, Tuple

# third-party
from vcorelib.io import ARBITER
from vcorelib.task import Inbox, Outbox
from vcorelib.task.manager import TaskManager
from vcorelib.task.subprocess.run import SubprocessLogMixin
from yambs.config import Config

# internal
from local.prompts import manual_select


def select_board_app(
    boards: Dict[str, Dict[str, str]], **kwargs
) -> Optional[Tuple[str, Path]]:
    """Select a board and application."""

    # Select a board.
    board = manual_select("board", boards, default=kwargs.get("board", ""))
    if board is None:
        return None

    # Select an app.
    app = manual_select("app", boards[board], default=kwargs.get("app", ""))
    if app is None:
        return None

    return board, Path(boards[board][app])


class Deploy(SubprocessLogMixin):
    """A task implementation for deploying firmware."""

    default_requirements = {"vmklib.init", "venv"}

    async def run(self, inbox: Inbox, outbox: Outbox, *args, **kwargs) -> bool:
        """Deploy new firmware."""

        config: Config = args[0]

        # Load 'deploy.json' if it's present.
        if config.root.joinpath("deploy.json").is_file():
            kwargs.update(
                ARBITER.decode(
                    config.root.joinpath("deploy.json"), require_success=True
                ).data
            )

        # Load board->apps data.
        board_apps = config.directory("ninja_out").joinpath("board_apps.json")

        # Generate the file if it doesn't exist yet.
        if not board_apps.is_file():
            assert await self.exec("mk", "-C", str(config.root), "g")

        result = select_board_app(
            ARBITER.decode(  # type: ignore
                board_apps, require_success=True
            ).data,
            **kwargs
        )
        if result is None:
            return False

        # Build the applications if they're not present.
        if not result[1].with_suffix(".elf").is_file():
            assert await self.exec("ninja")

        # Perform the update.
        if "pico" in result[0]:
            assert await self.exec(
                "picotool", "load", str(result[1].with_suffix(".uf2"))
            )
            assert await self.exec("picotool", "reboot")

        return True


def register_deploy(manager: TaskManager, config: Config) -> bool:
    """Register deploy tasks."""

    manager.register(Deploy("d", config), ["deps"])
    return True
