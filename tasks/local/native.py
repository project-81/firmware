"""
A module for interacting with native builds.
"""

# third-party
from vcorelib.task import Inbox, Outbox
from vcorelib.task.manager import TaskManager
from yambs.config import Config

# internal
from local.boards import BoardAppMixin


class RunNativeApp(BoardAppMixin):
    """A task implementation for deploying firmware."""

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

        exec_result = await self.exec(str(result[1].with_suffix(".elf")))
        self.log.info("'%s' completed (%s).", result[1].name, result)

        return exec_result


def register_native(manager: TaskManager, config: Config) -> bool:
    """Register native tasks."""

    manager.register(RunNativeApp("n", config), [])
    return True
