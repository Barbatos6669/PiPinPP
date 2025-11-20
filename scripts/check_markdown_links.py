#!/usr/bin/env python3
"""
Check all markdown links in documentation.

This script verifies that all internal links in markdown files point to
existing files. It ignores links inside code blocks to avoid false positives.

Usage:
    python3 scripts/check_markdown_links.py

Exit codes:
    0 - All links are valid
    1 - Broken links found
"""

import re
import sys
from pathlib import Path
from typing import List, Tuple


def extract_links(file_path: Path) -> List[Tuple[str, str, int]]:
    """
    Extract all markdown links from a file, excluding code blocks.
    
    Args:
        file_path: Path to markdown file
        
    Returns:
        List of (link_text, link_url, line_number) tuples
    """
    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Remove code blocks (both ``` and indented)
    # Remove triple-backtick code blocks
    content_no_code = re.sub(r'```.*?```', '', content, flags=re.DOTALL)
    
    # Remove inline code
    content_no_code = re.sub(r'`[^`]+`', '', content_no_code)
    
    links = []
    # Standard markdown links: [text](url)
    for match in re.finditer(r'\[([^\]]+)\]\(([^\)]+)\)', content_no_code):
        link_text = match.group(1)
        link_url = match.group(2)
        # Find line number in original content
        try:
            match_pos = content.index(match.group(0))
            line_num = content[:match_pos].count('\n') + 1
        except ValueError:
            line_num = -1
        links.append((link_text, link_url, line_num))
    
    return links


def resolve_link(source_file: Path, link_url: str) -> Tuple[Path, str]:
    """
    Resolve a link relative to source file.
    
    Args:
        source_file: Path to source markdown file
        link_url: URL from markdown link
        
    Returns:
        Tuple of (resolved_path, link_type)
        link_type is one of: 'local', 'external', 'mailto', 'anchor-only'
    """
    # Remove anchor from URL
    url_without_anchor = link_url.split('#')[0]
    
    # Skip external links
    if url_without_anchor.startswith(('http://', 'https://', 'ftp://')):
        return None, 'external'
    
    # Skip mailto links
    if url_without_anchor.startswith('mailto:'):
        return None, 'mailto'
    
    # Skip empty links (anchor only)
    if not url_without_anchor or url_without_anchor == '':
        return None, 'anchor-only'
    
    # Resolve relative path
    source_dir = source_file.parent
    target_path = (source_dir / url_without_anchor).resolve()
    
    return target_path, 'local'


def main():
    """Main function to check all markdown links."""
    # Get repository root (parent of scripts directory)
    script_dir = Path(__file__).parent
    repo_root = script_dir.parent
    docs_dir = repo_root / 'docs'
    
    broken_links = []
    all_links = []
    
    # Find all markdown files in docs/ and README
    md_files = list(docs_dir.rglob('*.md'))
    readme = repo_root / 'README.md'
    if readme.exists():
        md_files.append(readme)
    
    # Check links in each file
    for md_file in sorted(md_files):
        links = extract_links(md_file)
        
        for link_text, link_url, line_num in links:
            all_links.append((md_file, link_text, link_url))
            
            target_path, link_type = resolve_link(md_file, link_url)
            
            if link_type == 'local':
                if not target_path.exists():
                    broken_links.append({
                        'source': md_file,
                        'link_text': link_text,
                        'link_url': link_url,
                        'resolved_path': target_path,
                        'line_num': line_num,
                        'reason': 'File not found'
                    })
    
    # Print report
    print("=" * 80)
    print("MARKDOWN LINK VERIFICATION REPORT")
    print("=" * 80)
    print(f"\nTotal markdown files checked: {len(md_files)}")
    print(f"Total links found: {len(all_links)}")
    print(f"Broken links found: {len(broken_links)}")
    print()
    
    if broken_links:
        print("=" * 80)
        print("BROKEN LINKS:")
        print("=" * 80)
        
        # Group by source file
        by_file = {}
        for broken in broken_links:
            source = broken['source']
            if source not in by_file:
                by_file[source] = []
            by_file[source].append(broken)
        
        for source_file, issues in sorted(by_file.items()):
            rel_source = source_file.relative_to(repo_root)
            print(f"\n📄 {rel_source}")
            print(f"   {len(issues)} broken link(s):")
            
            for broken in issues:
                if broken['line_num'] > 0:
                    print(f"\n   Line {broken['line_num']}:")
                else:
                    print(f"\n   Location unknown:")
                print(f"   Link: [{broken['link_text']}]({broken['link_url']})")
                try:
                    print(f"   Target: {broken['resolved_path'].relative_to(repo_root)}")
                except ValueError:
                    # Path is not relative to repo root (e.g., absolute path)
                    print(f"   Target: {broken['resolved_path']}")
                print(f"   Status: ❌ {broken['reason']}")
        
        print("\n" + "=" * 80)
        print("SUMMARY:")
        print("=" * 80)
        
        # List missing files
        missing_files = set()
        for broken in broken_links:
            target = broken['resolved_path'].relative_to(repo_root)
            missing_files.add(str(target))
        
        print(f"\nMissing files ({len(missing_files)}):")
        for missing in sorted(missing_files):
            print(f"  ❌ {missing}")
        
        return 1
    else:
        print("✅ All local links are valid!")
        return 0


if __name__ == '__main__':
    sys.exit(main())
