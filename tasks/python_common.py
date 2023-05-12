"""
A module for project-specific task registration for Python projects.
"""

# third-party
from vcorelib.task import Inbox, Outbox
from vcorelib.task.subprocess.run import SubprocessLogMixin
from vmklib.tasks.mixins.concrete import ConcreteOnceMixin


class ThirdPartyEditable(ConcreteOnceMixin, SubprocessLogMixin):
    """A task for running an editable-install of a Python project."""

    default_requirements = {"vmklib.init", "venv"}

    async def run(self, inbox: Inbox, outbox: Outbox, *args, **kwargs) -> bool:
        """Editable-install a third-party dependency."""

        return await self.exec(
            str(inbox["venv"]["venv{python_version}"]["pip"]),
            "install",
            "-e",
            args[0].joinpath(kwargs["project"]),
        )
