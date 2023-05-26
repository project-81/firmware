"""
A module implementing interfaces for prompting the user for input.
"""

# built-in
from typing import Dict, Iterable, Optional


def select_option(
    options: Iterable[str],
    prompt: str = "selection? ",
    print_options: bool = False,
) -> Optional[int]:
    """
    Given a list of options, prompt user until they select a valid index and
    return it.
    """

    options = list(options)

    assert len(options) > 0
    if len(options) == 1:
        return 0

    if print_options:
        print("options:")
        for idx, option in enumerate(options):
            print(f"  {idx}: '{option}'")

    selection: int = -1
    while selection < 0 or selection >= len(options):
        try:
            value = input(prompt)
            selection = int(value)
        except ValueError:
            if value in options:
                selection = options.index(value)
        except KeyboardInterrupt:
            return None
    return selection


def manual_select(
    label: str,
    options: Iterable[str],
    default: str = "",
    allow_prompt: bool = True,
    custom_option: bool = False,
    descriptions: Dict[str, str] = None,
) -> Optional[str]:
    """
    Prompt the user until an option from the provided set of options is chosen.
    """

    options = list(options)

    custom_idx = -1
    if custom_option:
        options.append("<enter manually>")
        custom_idx = len(options) - 1

    if not options:
        print(f"no options for '{label}'!")
        return None

    # select the default if it was provided and valid, or if custom options are
    # enabled and a non-empty default was provided
    if default and ((default in options or not allow_prompt) or custom_option):
        return default

    print(f"options for '{label}':")
    for idx, option in enumerate(options):
        item_str = f"  {idx}: '{option}'"
        if descriptions and option in descriptions:
            item_str += f" ({descriptions[option]})"
        print(item_str)

    selection = None

    if not allow_prompt:
        print(f"not prompting and '{default}' not in options")
    else:
        selection = select_option(options)

    if selection is None or selection < 0:
        return None

    if selection == custom_idx:
        if not allow_prompt:
            print("error: selected manual entry option but prompts disabled")
            return None
        return str(input("value: "))

    return options[selection]
