"""
A module implementing some task mixins.
"""

# built-in
from pathlib import Path
from typing import Dict, List, Optional, Tuple

# third-party
from vcorelib.io import ARBITER
from vcorelib.task.subprocess.run import SubprocessLogMixin
from yambs.config import Config

# internal
from local.prompts import manual_select


class ShellCmdInDirMixin(SubprocessLogMixin):
    """
    A mixin that adds a method for running a shell command from a specific
    directory.
    """

    async def shell_cmd_in_dir(self, path: Path, cmd: List[str]) -> bool:
        """Run a shell command in a specific directory."""

        path.mkdir(exist_ok=True)
        return await self.shell(f'( cd "{path}"; {" ".join(cmd)} )')


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


class BoardAppMixin(ShellCmdInDirMixin):
    """A class mixin for facilitating board and application selection."""

    default_requirements = {"vmklib.init", "venv"}

    async def select_board_app(
        self, config: Config, deploy_json: bool = True, **kwargs
    ) -> Optional[Tuple[str, Path]]:
        """Select a board and application."""

        # Load 'deploy.json' if it's present.
        if deploy_json:
            if config.root.joinpath("deploy.json").is_file():
                kwargs.update(
                    ARBITER.decode(
                        config.root.joinpath("deploy.json"),
                        require_success=True,
                    ).data
                )

        # Load board->apps data.
        board_apps = config.directory("ninja_out").joinpath("board_apps.json")

        # Generate the file if it doesn't exist yet.
        if not board_apps.is_file():
            assert await self.exec("mk", "-C", str(config.root), "g")

        return select_board_app(
            ARBITER.decode(  # type: ignore
                board_apps, require_success=True
            ).data,
            **kwargs,
        )
