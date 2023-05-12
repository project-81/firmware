"""
A module for project-specific task registration for Python projects.
"""

# built-in
from pathlib import Path

# third-party
from vcorelib.task import Inbox, Outbox
from vcorelib.task.manager import TaskManager
from vcorelib.task.subprocess.run import SubprocessLogMixin
from vmklib.tasks.mixins.concrete import ConcreteOnceMixin


class ThirdPartyEditable(ConcreteOnceMixin, SubprocessLogMixin):
    """A task for running an editable-install of a Python project."""

    default_requirements = {"vmklib.init", "venv", "third-party-clones"}

    async def run(self, inbox: Inbox, outbox: Outbox, *args, **kwargs) -> bool:
        """Editable-install a third-party dependency."""

        return await self.exec(
            str(inbox["venv"]["venv{python_version}"]["pip"]),
            "install",
            "-e",
            args[0].joinpath(kwargs["pyproject"]),
        )


class ThirdPartyNox(ConcreteOnceMixin, SubprocessLogMixin):
    """A task for running an editable-install of a Python project."""

    default_requirements = {"vmklib.init", "venv", "third-party-clones"}

    async def run(self, inbox: Inbox, outbox: Outbox, *args, **kwargs) -> bool:
        """Editable-install a third-party dependency."""

        noxfile = args[0].joinpath(kwargs["pyproject"], "noxfile.py")

        return await self.exec(
            str(inbox["venv"]["venv{python_version}"]["bin"].joinpath("nox")),
            "-f",
            str(noxfile),
            "-s",
            kwargs["session"],
        )


def register_python(manager: TaskManager, third_party: Path) -> bool:
    """Register git tasks."""

    manager.register(
        ThirdPartyEditable("third-party-editable-{pyproject}", third_party), []
    )
    manager.register(
        ThirdPartyNox("third-party-nox-{pyproject}-{session}", third_party),
        ["build-node"],
    )

    return True
