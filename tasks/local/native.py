"""
A module for interacting with native builds.
"""

# built-in
import asyncio
from pathlib import Path
from sys import executable
from typing import Iterable

# third-party
from vcorelib.task import Inbox, Outbox
from vcorelib.task.manager import TaskManager
from yambs.config import Config

# internal
from local.mixin import BoardAppMixin


class RunNativeApp(BoardAppMixin):
    """A task implementation for deploying firmware."""

    async def generate_coverage(
        self,
        program: Path,
        output_root: Path,
        extra_coverage_dirs: Iterable[Path] = None,
    ) -> None:
        """
        Attempt to locate code-coverage data related to the provided program.
        """

        html_out = output_root.joinpath("htmlcov")
        html_out.mkdir(exist_ok=True, parents=True)

        # Aggregate coverage files.
        coverage_files = [program.with_suffix(".gcda")]

        if extra_coverage_dirs is None:
            extra_coverage_dirs = []

        # Add additional coverage directories.
        for extra_dir in extra_coverage_dirs:
            for item in extra_dir.iterdir():
                print(item)
                if item.name.endswith("gcda"):
                    coverage_files.append(item)

        coverage_map = {
            x: output_root.joinpath(x.with_suffix(".info").name)
            for x in coverage_files
            if x.is_file()
        }

        # Generate coverage info.
        await asyncio.gather(
            *[
                self.exec("geninfo", str(coverage_in), "-o", str(coverage_out))
                for coverage_in, coverage_out in coverage_map.items()
            ]
        )

        # Generate HTML.
        if not await self.exec(
            "genhtml", *set(coverage_map.values()), "-o", str(html_out)
        ):
            return

        # Host HTML via built-in server.
        await self.shell_cmd_in_dir(
            html_out, [executable, "-m", "http.server", "0"]
        )

    async def run(self, inbox: Inbox, outbox: Outbox, *args, **kwargs) -> bool:
        """Run a native application."""

        config: Config = args[0]

        board = "native"

        result = await self.select_board_app(
            config,
            board=board,
            app=kwargs.get("app", ""),
            deploy_json=False,
        )
        if result is None:
            return False

        # Ideally we would remove coverage data from previous runs here.

        entry = result[1].with_suffix(".elf")
        exec_result = await self.exec(str(entry))
        self.log.info("'%s' completed (%s).", entry.name, exec_result)

        # Generate coverage.
        if exec_result and kwargs.get("coverage", False):
            build = Path("build")

            await self.generate_coverage(
                entry,
                build.joinpath("coverage"),
                extra_coverage_dirs=[
                    build.joinpath(board, board, board, "common", "cobs")
                ],
            )

        return exec_result


def register_native(manager: TaskManager, config: Config) -> bool:
    """Register native tasks."""

    deps = ["gb"]

    manager.register(RunNativeApp("n", config), deps)
    manager.register(RunNativeApp("nc", config, coverage=True), deps)
    return True
