"""
A module implementing tasks for building node.
"""

# built-in
from os import environ
from pathlib import Path

# third-party
from vcorelib.task.manager import TaskManager

# internal
from local.common import add_path, add_program_path, program_str
from local.configure import ThirdPartyConfigure


def register_node(manager: TaskManager, third_party: Path) -> bool:
    """Register node targets."""

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
        ["build-ninja", "github-shallow-clone.nodejs.node-v20.1.0"],
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
