"""
A module for interacting with native builds.
"""

# built-in
from pathlib import Path
from sys import executable

# third-party
from vcorelib.task import Inbox, Outbox
from vcorelib.task.manager import TaskManager
from yambs.config import Config

# internal
from local.mixin import BoardAppMixin


class RunNativeApp(BoardAppMixin):
    """A task implementation for deploying firmware."""

    async def generate_coverage(
        self, program: Path, output_root: Path
    ) -> None:
        """
        Attempt to locate code-coverage data related to the provided program.
        """

        coverage = program.with_suffix(".gcda")

        if not coverage.is_file():
            return

        html_out = output_root.joinpath("htmlcov")
        html_out.mkdir(exist_ok=True, parents=True)

        coverage_out = output_root.joinpath(program.with_suffix(".info").name)

        # Generate coverage info.
        await self.exec("geninfo", str(coverage), "-o", str(coverage_out))

        # Generate HTML.
        await self.exec("genhtml", str(coverage_out), "-o", str(html_out))

        # Host HTML via built-in server.
        await self.shell_cmd_in_dir(
            html_out, [executable, "-m", "http.server", "0"]
        )

    async def run(self, inbox: Inbox, outbox: Outbox, *args, **kwargs) -> bool:
        """Run a native application."""

        config: Config = args[0]

        result = await self.select_board_app(
            config,
            board="native",
            app=kwargs.get("app", ""),
            deploy_json=False,
        )
        if result is None:
            return False

        entry = result[1].with_suffix(".elf")
        exec_result = await self.exec(str(entry))
        self.log.info("'%s' completed (%s).", entry.name, exec_result)

        # Generate coverage.
        if exec_result and kwargs.get("coverage", False):
            await self.generate_coverage(entry, Path("build", "coverage"))

        return exec_result


def register_native(manager: TaskManager, config: Config) -> bool:
    """Register native tasks."""

    deps = ["gb"]

    manager.register(RunNativeApp("n", config), deps)
    manager.register(RunNativeApp("nc", config, coverage=True), deps)
    return True
