import json
import argparse
from typing import Union, Dict, Tuple


def process_transform(obj: dict, key: str, swap_axes: Tuple[str, str], scale: float,
                      translation: Tuple[float, float, float], counters: Dict[str, int],
                      do_swap: bool, do_scale: bool, do_translate: bool):
    """Apply transformation rules based on key type."""
    if all(k in obj for k in ("x", "y", "z")) and all(isinstance(obj[k], (int, float)) for k in ("x", "y", "z")):
        # Apply translation first (only for position)
        if key == "position" and do_translate:
            obj["x"] += translation[0]
            obj["y"] += translation[1]
            obj["z"] += translation[2]

        # Swap chosen axes
        if do_swap:
            a, b = swap_axes
            obj[a], obj[b] = obj[b], obj[a]

        # Apply scaling (position + scale only)
        if do_scale and key in ("position", "scale"):
            obj["x"] *= scale
            obj["y"] *= scale
            obj["z"] *= scale

        counters[key] += 1


def traverse(obj: Union[dict, list], swap_axes: Tuple[str, str], scale: float,
             translation: Tuple[float, float, float], counters: Dict[str, int],
             do_swap: bool, do_scale: bool, do_translate: bool):
    """Recursively traverse JSON structure and apply transforms."""
    if isinstance(obj, dict):
        for key in ("position", "rotation", "scale"):
            if key in obj and isinstance(obj[key], dict):
                process_transform(obj[key], key, swap_axes, scale, translation, counters,
                                  do_swap, do_scale, do_translate)

        for value in obj.values():
            traverse(value, swap_axes, scale, translation, counters, do_swap, do_scale, do_translate)

    elif isinstance(obj, list):
        for item in obj:
            traverse(item, swap_axes, scale, translation, counters, do_swap, do_scale, do_translate)


def detect_indent(file_path: str) -> int:
    """Try to detect indentation from the first indented line in a JSON file."""
    with open(file_path, "r", encoding="utf-8") as f:
        for line in f:
            stripped = line.lstrip()
            if stripped and stripped[0] in ('"', '{', '[') and len(line) - len(stripped) > 0:
                return len(line) - len(stripped)
    return 2  # fallback


def generate_output_filename(file_path: str, do_swap: bool, swap_axes: Tuple[str, str],
                             do_scale: bool, scale: float, do_translate: bool,
                             translation: Tuple[float, float, float]) -> str:
    """Generate a filename summarizing the transformations applied."""
    base_name = file_path.rsplit(".", 1)[0]
    parts = []

    if do_swap:
        parts.append(f"axis_{swap_axes[0]}{swap_axes[1]}")
    if do_scale:
        parts.append(f"scale_{scale}")
    if do_translate:
        parts.append(f"translate_{translation[0]}_{translation[1]}_{translation[2]}")

    if parts:
        return f"{base_name}_{'_'.join(parts)}.json"
    else:
        return None  # no transformations applied


def process_json_file(file_path: str, swap_axes: Tuple[str, str], scale: float,
                      translation: Tuple[float, float, float],
                      do_swap: bool, do_scale: bool, do_translate: bool):
    # If no transformations are requested, print message and exit
    if not (do_swap or do_scale or do_translate):
        print("No transformations specified. Nothing to do.")
        return

    with open(file_path, "r", encoding="utf-8") as f:
        data = json.load(f)

    counters = {"position": 0, "rotation": 0, "scale": 0}

    traverse(data, swap_axes, scale, translation, counters, do_swap, do_scale, do_translate)

    # Generate descriptive output filename
    output_path = generate_output_filename(file_path, do_swap, swap_axes, do_scale, scale, do_translate, translation)

    # Preserve original indentation style
    indent = detect_indent(file_path)

    with open(output_path, "w", encoding="utf-8") as f:
        json.dump(data, f, indent=indent, ensure_ascii=False)

    print("Number of objects processed:")
    for key, count in counters.items():
        msg_parts = []
        if do_translate and key == "position":
            msg_parts.append(f"translated {translation}")
        if do_swap:
            msg_parts.append(f"swapped {swap_axes[0]}/{swap_axes[1]}")
        if do_scale and key in ("position", "scale"):
            msg_parts.append(f"scaled by {scale}")
        msg_str = ", ".join(msg_parts) if msg_parts else "no change"
        print(f"  {key}: {count} ({msg_str})")
    print(f"Output JSON written to: {output_path}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Transform JSON file with optional axis swap, scaling, and translation.")
    parser.add_argument("json_file", help="Path to the JSON file")
    parser.add_argument("--axis", nargs=2, metavar=("AXIS1", "AXIS2"),
                        help="Two axes to swap (choices: x y z)")
    parser.add_argument("--scale", type=float, default=1.0,
                        help="Scaling factor for position and scale objects (default: 1.0)")
    parser.add_argument("--translation", nargs=3, type=float, default=(0.0, 0.0, 0.0),
                        metavar=("TX", "TY", "TZ"),
                        help="Translation vector for position objects (default: 0 0 0)")

    args = parser.parse_args()

    do_swap = args.axis is not None
    swap_axes = tuple(args.axis) if do_swap else ("x", "z")  # dummy if not used
    do_scale = args.scale != 1.0
    do_translate = tuple(args.translation) != (0.0, 0.0, 0.0)

    process_json_file(args.json_file, swap_axes, args.scale, tuple(args.translation),
                      do_swap, do_scale, do_translate)
