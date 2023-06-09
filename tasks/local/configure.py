"""
A module implementing tasks for building projects that require ./configure'ing.
"""

# built-in
from multiprocessing import cpu_count
from pathlib import Path

# third-party
from vcorelib.task import Inbox, Outbox
from vmklib.tasks.mixins.concrete import ConcreteOnceMixin

# internal
from local.mixin import ShellCmdInDirMixin

MAKE_NPROCS = f"-j{cpu_count()}"


class ThirdPartyConfigure(ConcreteOnceMixin, ShellCmdInDirMixin):
    """A class implementing a task for building './configure' projects."""

    default_requirements = {"vmklib.init", "third-party-clones"}

    async def run(self, inbox: Inbox, outbox: Outbox, *args, **kwargs) -> bool:
        """Build a third-party dependency."""

        root = args[0].joinpath(kwargs["project"])

        configure = kwargs.get("configure", "configure")

        result = True

        # Bootstrap.
        if not Path(root, configure).is_file():
            result &= await self.shell_cmd_in_dir(
                root,
                [f"./{kwargs.get('bootstrap', 'bootstrap')}"]
                + kwargs.get("bootstrap_args", []),
            )

        cmd = [f"./{configure}", *args[1:]]
        result &= await self.shell_cmd_in_dir(root, cmd)
        if not result:
            return False

        if kwargs.get("make", False):
            result &= await self.shell_cmd_in_dir(root, ["make", MAKE_NPROCS])

        if "ninja" in kwargs:
            result &= await self.shell_cmd_in_dir(
                root, [kwargs["ninja"]] + kwargs.get("ninja_args", [])
            )

        return result
