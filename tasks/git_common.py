"""
A module for working with git.
"""

# built-in
from enum import Enum
from pathlib import Path

# third-party
from vcorelib.task import Inbox, Outbox
from vcorelib.task.manager import TaskManager
from vcorelib.task.subprocess.run import SubprocessLogMixin


class GitSource(Enum):
    """An enumeration for possible internet git-repository sources."""

    GITHUB = {
        "ssh": "git@github.com:{path}/{project}.git",
        "https": "https://github.com/{path}/{firmware}.git",
    }

    def url(self, path: str, project: str, method: str = "ssh") -> str:
        """Construct a URL from parameters."""
        return self.value[method].format(path=path, project=project)

    @staticmethod
    def from_str(source: str) -> "GitSource":
        """Get a source from a string name."""
        return {"github": GitSource.GITHUB}[source]


class CloneGitRepo(SubprocessLogMixin):
    """A task implelemtation for cloning a git repository."""

    async def run(self, inbox: Inbox, outbox: Outbox, *args, **kwargs) -> bool:
        """Create or update a project's virtual environment."""

        path = kwargs["path"]
        cwd: Path = args[0].joinpath(path)
        project = kwargs["project"]

        dest = cwd.joinpath(project)
        outbox["project"] = dest

        # Check if the repository is already cloned.
        if dest.is_dir():
            return True

        source = GitSource.from_str(kwargs.get("source", "github"))

        cli_args = ["-C", str(cwd), "clone", *args[1:]]

        if "branch" in kwargs:
            cli_args.append("--branch")
            cli_args.append(kwargs["branch"])

        cli_args.append(source.url(path, project))

        cwd.mkdir(parents=True, exist_ok=True)

        result = await self.exec("git", *cli_args)

        # Create a symbolic link to the root of the third-party root directory.
        if result:
            symlink = args[0].joinpath(project)
            if not symlink.exists():
                symlink.symlink_to(dest, target_is_directory=True)

        return result


def register_git(manager: TaskManager, third_party: Path) -> bool:
    """Register git tasks."""

    manager.register(
        CloneGitRepo("github-clone.{path}.{project}", third_party), []
    )
    manager.register(
        CloneGitRepo(
            "github-shallow-clone.{path}.{project}.{branch}",
            third_party,
            "--depth=1",
            "--single-branch",
        ),
        [],
    )

    return True
