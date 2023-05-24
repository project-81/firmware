"""
A module implementing tasks for RP2040 boards.
"""

# built-in
from pathlib import Path

# third-party
from vcorelib.task import Inbox, Outbox
from vmklib.tasks.mixins.concrete import ConcreteOnceMixin

# internal
from local.configure import ShellCmdInDirMixin

PIOASM_DIR = ["raspberrypi", "pico-sdk", "tools", "pioasm"]


class BuildPioasm(ConcreteOnceMixin, ShellCmdInDirMixin):
    """A task for ensuring pioasm is built and available for use."""

    async def run(self, inbox: Inbox, outbox: Outbox, *args, **kwargs) -> bool:
        """Build pioasm."""

        third_party: Path = args[0]

        pioasm_dir = third_party.joinpath(*PIOASM_DIR)
        build_dir = pioasm_dir.joinpath("build")

        result = True

        pioasm = build_dir.joinpath("pioasm")

        # Ensure the program is built.
        if not pioasm.is_file():
            result &= await self.shell_cmd_in_dir(build_dir, ["cmake", ".."])
            result &= await self.shell_cmd_in_dir(build_dir, ["ninja"])

        return result
