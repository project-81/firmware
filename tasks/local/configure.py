"""
A module implementing tasks for building projects that require ./configure'ing.
"""

# built-in
from os import environ
from pathlib import Path
from typing import Dict, List

# third-party
from vcorelib.task import Inbox, Outbox
from vcorelib.task.manager import TaskManager
from vcorelib.task.subprocess.run import SubprocessLogMixin
from vmklib.tasks.mixins.concrete import ConcreteOnceMixin

# internal
from local.common import add_path, add_program_path, program_str


class ThirdPartyConfigure(ConcreteOnceMixin, SubprocessLogMixin):
    """A class implementing a task for building './configure' projects."""

    default_requirements = {"vmklib.init", "third-party-clones"}

    async def shell_cmd_in_dir(self, path: Path, cmd: List[str]) -> bool:
        """Run a shell command in a specific directory."""
        return await self.shell(f'( cd "{path}"; {" ".join(cmd)} )')

    async def run(self, inbox: Inbox, outbox: Outbox, *args, **kwargs) -> bool:
        """Editable-install a third-party dependency."""

        root = args[0].joinpath(kwargs["project"])

        configure = kwargs.get("configure", "configure")
        cmd = [f"./{configure}", *args[1:]]
        result = await self.shell_cmd_in_dir(root, cmd)
        if not result:
            return False

        if kwargs.get("make", False):
            result &= await self.shell_cmd_in_dir(root, ["make"])

        if "ninja" in kwargs:
            result &= await self.shell_cmd_in_dir(
                root, [kwargs["ninja"]] + kwargs.get("ninja_args", [])
            )

        return result


def register_configure(
    manager: TaskManager, third_party: Path, substitutions: Dict[str, str]
) -> bool:
    """Register configure targets."""

    # Build ninja.
    manager.register(
        ThirdPartyConfigure(
            "build-ninja",
            third_party,
            "--bootstrap",
            project="ninja",
            ninja="ninja",
            configure="configure.py",
        ),
        [],
    )

    # A path to the ninja binary.
    add_program_path("ninja", third_party, "ninja", "ninja", update_path=True)

    # Build node.
    manager.register(
        ThirdPartyConfigure(
            "build-node",
            third_party,
            "--ninja",
            project="node",
            ninja=program_str("ninja"),
            ninja_args=["-C", "out/Release"],
        ),
        ["build-ninja"],
    )

    node_root = third_party.joinpath("node")

    # A path to the node binary.
    add_program_path(
        "node", node_root, "out", "Release", "node", update_path=True
    )
    environ["NODE_OPTIONS"] = "--openssl-legacy-provider"

    # Add shims to the path.
    add_path(node_root.joinpath("deps", "corepack", "shims"))

    return True
