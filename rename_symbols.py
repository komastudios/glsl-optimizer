#!/usr/bin/env python3
"""
Symbol Renamer for C/C++ Source Files

Renames symbols in source files by adding prefixes, handling symbols that
appear with a leading underscore in the symbols file but without it in the source.
"""

import os
import sys
import re
import shutil
import logging
from typing import Set, Dict, Optional
from pathlib import Path

class SymbolRenamer:
    def __init__(self, symbols_file: str, prefix: str, verbose: bool = False):
        self.prefix = prefix
        self.verbose = verbose
        self._setup_logging()
        self.symbols = self._load_symbols(symbols_file)
        self.symbol_map = self._create_symbol_map()

        # Sort symbols by length (longest first) to avoid partial matches
        self.sorted_symbols = sorted(self.symbols, key=len, reverse=True)

        # Common C/C++ keywords to avoid renaming
        self.keywords = {
            'if', 'else', 'while', 'for', 'do', 'return', 'break', 'continue',
            'switch', 'case', 'default', 'goto', 'sizeof', 'typedef',
            'void', 'char', 'short', 'int', 'long', 'float', 'double',
            'signed', 'unsigned', 'struct', 'union', 'enum', 'class',
            'public', 'private', 'protected', 'template', 'namespace'
        }

    def _setup_logging(self) -> None:
        level = logging.DEBUG if self.verbose else logging.INFO
        logging.basicConfig(
            level=level,
            format='%(asctime)s - %(levelname)s - %(message)s',
            datefmt='%Y-%m-%d %H:%M:%S'
        )

    def _load_symbols(self, filename: str) -> Set[str]:
        """
        Load symbols from file, processing only those starting with underscore.
        Removes the leading underscore as these symbols appear without it in source.

        Args:
            filename: Path to symbols file

        Returns:
            Set of symbols without leading underscores
        """
        with open(filename, 'r') as f:
            # Only process lines starting with underscore and remove it
            return {line.strip()[1:] for line in f
                    if line.strip() and line.strip().startswith('_')}

    def _create_symbol_map(self) -> Dict[str, str]:
        """
        Create mapping from original symbols (without underscore) to prefixed versions.

        Returns:
            Dictionary mapping original symbols to their prefixed versions
        """
        return {s: f"{self.prefix}{s}" for s in self.symbols}

    def _is_valid_context(self, content: str, start: int, end: int) -> bool:
        """
        Check if the symbol occurrence is in a valid context for renaming.

        Args:
            content: Source file content
            start: Start position of symbol
            end: End position of symbol

        Returns:
            True if the context is valid for renaming
        """
        # Check previous character (if any) is not alphanumeric or underscore
        if start > 0:
            prev_char = content[start - 1]
            if prev_char.isalnum() or prev_char == '_':
                return False

        # Check next character (if any) is not alphanumeric or underscore
        if end < len(content):
            next_char = content[end]
            if next_char.isalnum() or next_char == '_':
                return False

        return True

    def process_file(self, filepath: str, out_filepath: str) -> None:
        """
        Process a single source file, replacing symbols as needed.

        Args:
            filepath: Path to input file
            out_filepath: Path to output file
        """
        logging.debug(f"Processing file: {filepath}")

        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()

        modified_content = content

        # Process each symbol
        for symbol in self.sorted_symbols:
            if symbol in self.keywords:
                continue

            # Create pattern that matches the symbol with word boundaries
            pattern = r'\b' + re.escape(symbol) + r'\b'

            def replace_func(match):
                start = match.start()
                end = match.end()

                if self._is_valid_context(modified_content, start, end):
                    replacement = self.symbol_map[symbol]
                    logging.debug(f"Replacing '{match.group(0)}' with '{replacement}'")
                    return replacement
                return match.group(0)

            modified_content = re.sub(pattern, replace_func, modified_content)

        # Write the modified content
        os.makedirs(os.path.dirname(out_filepath), exist_ok=True)
        with open(out_filepath, 'w', encoding='utf-8') as f:
            f.write(modified_content)

        logging.debug(f"Wrote modified content to: {out_filepath}")

    def process_directory(self, src_dir: str, dst_dir: str) -> None:
        """
        Process all source files in a directory tree.

        Args:
            src_dir: Source directory
            dst_dir: Destination directory
        """
        src_dir = Path(src_dir)
        dst_dir = Path(dst_dir)

        logging.info(f"Processing directory: {src_dir}")
        logging.info(f"Output directory: {dst_dir}")

        for src_path in src_dir.rglob('*'):
            if src_path.is_file():
                rel_path = src_path.relative_to(src_dir)
                dst_path = dst_dir / rel_path

                if src_path.suffix in {'.c', '.cpp', '.h', '.hpp', '.cc', '.hxx', '.cxx'}:
                    logging.info(f"Processing: {rel_path}")
                    self.process_file(str(src_path), str(dst_path))
                else:
                    logging.debug(f"Copying: {rel_path}")
                    dst_path.parent.mkdir(parents=True, exist_ok=True)
                    shutil.copy2(src_path, dst_path)

def main() -> None:
    """Main entry point for the script."""
    import argparse

    parser = argparse.ArgumentParser(
        description='Rename symbols in source files by adding prefixes')
    parser.add_argument('symbols_file', help='File containing symbols to rename')
    parser.add_argument('prefix', help='Prefix to add to symbols')
    parser.add_argument('source_dir', help='Source directory')
    parser.add_argument('dest_dir', help='Destination directory')
    parser.add_argument('--verbose', '-v', action='store_true',
                        help='Enable verbose output')

    args = parser.parse_args()

    renamer = SymbolRenamer(args.symbols_file, args.prefix, args.verbose)
    renamer.process_directory(args.source_dir, args.dest_dir)

if __name__ == "__main__":
    main()