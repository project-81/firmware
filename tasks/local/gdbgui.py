"""
A module for project-specific gdbgui tasks.
"""

# built-in
from pathlib import Path

# third-party
from vcorelib.task import Inbox, Outbox, Phony
from vcorelib.task.manager import TaskManager
from vcorelib.task.subprocess.run import SubprocessLogMixin

# internal
from local.common import PATHS


class Gdbgui(SubprocessLogMixin):
    """A task for running a gdbgui instance."""

    async def run(self, inbox: Inbox, outbox: Outbox, *args, **kwargs) -> bool:
        """Build a third-party dependency."""

        entry = args[0].joinpath("gdbgui", "build", "executable", "gdbgui.pex")

        toolchain = kwargs["toolchain"]
        self.log.info("Toolchain: '%s'.", toolchain)

        architecture = kwargs.get("architecture", "armv7e-m+fp")
        self.log.info("Architecture: '%s'.", architecture)

        cpu = kwargs.get("cpu", "cortex-m4")
        self.log.info("CPU: '%s'.", cpu)

        app = kwargs.get("app", "common/test1.elf")
        self.log.info("App: '%s'.", app)

        # Build a path to the application.
        app_path = PATHS["build"].joinpath(
            toolchain, architecture, cpu, "apps", app
        )

        if not app_path.is_file():
            self.log.error("'%s' doesn't exist.", app_path)
            return False

        return await self.exec(str(entry), "-g", f"{toolchain}-gdb {app_path}")


def register_gdbgui(manager: TaskManager, third_party: Path) -> bool:
    """Register git tasks."""

    manager.register(
        Gdbgui("gdbgui-{toolchain}", third_party),
        ["third-party-nox-gdbgui-build_executables_current_platform"],
    )

    manager.register(Phony("gdbgui"), ["gdbgui-arm-picolibc-eabi"])

    return True
