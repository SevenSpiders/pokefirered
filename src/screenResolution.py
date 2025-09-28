#!/usr/bin/env python3
"""
Retro Handheld Screen Resolution Analyzer
Finds optimal screen resolutions for retro gaming with integer scaling
"""

import math
from collections import defaultdict

# Define retro gaming systems and their native resolutions
RETRO_SYSTEMS = {
    'Game Boy': (160, 144),
    'Game Boy Color': (160, 144),
    'Game Boy Advance': (240, 160),
    'SNES': (256, 224),
    'NES': (256, 240),
    'Neo Geo': (320, 224),
    'Genesis/Mega Drive': (320, 224),
    'PC Engine/TurboGrafx-16': (256, 224),
    'Master System': (256, 192),
    'Game Gear': (160, 144),
    'Atari Lynx': (160, 102),
    'Neo Geo Pocket': (160, 152),
    'WonderSwan': (224, 144),
    'PICO-8': (128, 128),
    'Amstrad CPC': (160, 200),
    'C64': (320, 200),
    'ZX Spectrum': (256, 192),
    'Atari 2600': (160, 192),
    'MSX': (256, 192),
    'Virtual Boy': (384, 224),
}

def calculate_scaling_info(screen_width, screen_height, native_width, native_height):
    """Calculate the best integer scaling factor and resulting screen usage"""
    # Calculate maximum integer scaling factors for width and height
    max_scale_x = screen_width // native_width
    max_scale_y = screen_height // native_height
    
    # Use the smaller scaling factor to maintain aspect ratio
    scale_factor = min(max_scale_x, max_scale_y)
    
    if scale_factor == 0:
        return None  # Screen too small for this system
    
    # Calculate scaled dimensions
    scaled_width = native_width * scale_factor
    scaled_height = native_height * scale_factor
    
    # Calculate screen usage percentage
    screen_area = screen_width * screen_height
    used_area = scaled_width * scaled_height
    usage_percentage = (used_area / screen_area) * 100
    
    return {
        'scale_factor': scale_factor,
        'scaled_width': scaled_width,
        'scaled_height': scaled_height,
        'usage_percentage': usage_percentage,
        'unused_width': screen_width - scaled_width,
        'unused_height': screen_height - scaled_height
    }

def analyze_resolutions(max_resolution=2048):
    """Analyze all possible screen resolutions up to max_resolution x max_resolution"""
    
    # Store results for analysis
    resolution_scores = defaultdict(list)
    best_resolutions = {}
    
    print(f"Analyzing screen resolutions up to {max_resolution}x{max_resolution}...")
    print(f"Checking {len(RETRO_SYSTEMS)} retro gaming systems\n")
    
    # Common aspect ratios for handheld screens
    common_ratios = [
        (4, 3),    # Classic 4:3
        (3, 2),    # 3:2 (Steam Deck, some handhelds)
        (16, 10),  # 16:10 (many laptops/handhelds)
        (5, 3),    # 5:3
        (16, 9),   # 16:9 (widescreen)
    ]
    
    resolutions_to_check = set()
    
    # Generate resolutions based on common aspect ratios
    for ratio_w, ratio_h in common_ratios:
        for base_size in range(50, max_resolution // max(ratio_w, ratio_h) + 1, 10):
            width = ratio_w * base_size
            height = ratio_h * base_size
            if width <= max_resolution and height <= max_resolution:
                resolutions_to_check.add((width, height))
    
    # Add some additional common resolutions
    additional_resolutions = [
        (480, 320), (640, 480), (800, 600), (1024, 768),
        (1280, 800), (1366, 768), (1920, 1080), (1280, 720),
        (854, 480), (960, 540), (1440, 900), (1680, 1050),
        # Handheld-specific resolutions
        (800, 480), (1280, 800), (1920, 1200), (2560, 1600),
        (480, 272), (960, 544), (1024, 600), (1200, 800),
    ]
    
    for res in additional_resolutions:
        if res[0] <= max_resolution and res[1] <= max_resolution:
            resolutions_to_check.add(res)
    
    resolutions_to_check = sorted(list(resolutions_to_check))
    
    print(f"Checking {len(resolutions_to_check)} potential screen resolutions...\n")
    
    # Analyze each resolution
    for screen_width, screen_height in resolutions_to_check:
        resolution_key = f"{screen_width}x{screen_height}"
        system_results = []
        
        for system_name, (native_width, native_height) in RETRO_SYSTEMS.items():
            scaling_info = calculate_scaling_info(screen_width, screen_height, native_width, native_height)
            
            if scaling_info:
                system_results.append({
                    'system': system_name,
                    'native_res': f"{native_width}x{native_height}",
                    **scaling_info
                })
        
        if system_results:
            # Calculate average usage percentage across all systems
            avg_usage = sum(result['usage_percentage'] for result in system_results) / len(system_results)
            
            resolution_scores[resolution_key] = {
                'resolution': (screen_width, screen_height),
                'avg_usage': avg_usage,
                'system_results': system_results
            }
    
    return resolution_scores

def print_top_resolutions(resolution_scores, top_n=10):
    """Print the top N resolutions with best average screen usage"""
    
    # Sort by average usage percentage
    sorted_resolutions = sorted(
        resolution_scores.items(), 
        key=lambda x: x[1]['avg_usage'], 
        reverse=True
    )
    
    print(f"🏆 TOP {top_n} SCREEN RESOLUTIONS FOR RETRO GAMING")
    print("=" * 60)
    
    for i, (resolution_key, data) in enumerate(sorted_resolutions[:top_n], 1):
        width, height = data['resolution']
        avg_usage = data['avg_usage']
        aspect_ratio = width / height
        
        print(f"\n#{i}. {resolution_key} (aspect ratio: {aspect_ratio:.2f}:1)")
        print(f"    Average screen usage: {avg_usage:.1f}%")
        print(f"    System breakdown:")
        
        # Sort systems by usage percentage for this resolution
        systems_sorted = sorted(data['system_results'], key=lambda x: x['usage_percentage'], reverse=True)
        
        for result in systems_sorted:
            print(f"      {result['system']:20} {result['native_res']:10} -> "
                  f"{result['scaled_width']:4}x{result['scaled_height']:<4} "
                  f"({result['scale_factor']}x scale, {result['usage_percentage']:5.1f}% used)")

def print_system_analysis(resolution_scores):
    """Print analysis for each system showing best resolutions"""
    
    print(f"\n\n🎮 BEST RESOLUTIONS BY GAMING SYSTEM")
    print("=" * 60)
    
    # Group by system
    system_best = defaultdict(list)
    
    for resolution_key, data in resolution_scores.items():
        for result in data['system_results']:
            system_best[result['system']].append({
                'resolution': resolution_key,
                'usage': result['usage_percentage'],
                'scale': result['scale_factor'],
                'scaled_res': f"{result['scaled_width']}x{result['scaled_height']}"
            })
    
    # Print best resolutions for each system
    for system in sorted(system_best.keys()):
        results = sorted(system_best[system], key=lambda x: x['usage'], reverse=True)
        native_res = next(f"{w}x{h}" for name, (w, h) in RETRO_SYSTEMS.items() if name == system)
        
        print(f"\n{system} (native: {native_res}):")
        
        # Show top 5 resolutions for this system
        for i, result in enumerate(results[:5]):
            print(f"  {i+1}. {result['resolution']:10} -> {result['scaled_res']:10} "
                  f"({result['scale']}x scale, {result['usage']:5.1f}% used)")

def main():
    """Main function to run the analysis"""
    
    print("🕹️  RETRO HANDHELD SCREEN RESOLUTION ANALYZER")
    print("=" * 50)
    print("Finding optimal screen resolutions for retro gaming with integer scaling\n")
    
    # Run the analysis
    resolution_scores = analyze_resolutions(max_resolution=2048)
    
    if not resolution_scores:
        print("No valid resolutions found!")
        return
    
    print(f"Analysis complete! Found {len(resolution_scores)} valid screen resolutions.\n")
    
    # Print results
    print_top_resolutions(resolution_scores, top_n=15)
    print_system_analysis(resolution_scores)
    
    # Additional insights
    print(f"\n\n📊 ADDITIONAL INSIGHTS")
    print("=" * 30)
    
    # Find resolutions with 100% usage for any system
    perfect_matches = []
    for resolution_key, data in resolution_scores.items():
        for result in data['system_results']:
            if result['usage_percentage'] >= 99.9:  # Account for floating point precision
                perfect_matches.append(f"{resolution_key} -> {result['system']}")
    
    if perfect_matches:
        print(f"\n🎯 Perfect matches (100% screen usage):")
        for match in sorted(set(perfect_matches)):
            print(f"  {match}")
    else:
        print(f"\n🎯 No perfect matches found (100% screen usage)")
    
    # Find most versatile resolutions (good for many systems)
    versatile_resolutions = []
    for resolution_key, data in resolution_scores.items():
        good_systems = sum(1 for result in data['system_results'] if result['usage_percentage'] >= 70)
        if good_systems >= len(RETRO_SYSTEMS) * 0.6:  # Good for 60%+ of systems
            versatile_resolutions.append((resolution_key, good_systems, data['avg_usage']))
    
    if versatile_resolutions:
        versatile_resolutions.sort(key=lambda x: (x[1], x[2]), reverse=True)
        print(f"\n🌟 Most versatile resolutions (good for many systems):")
        for res, count, avg in versatile_resolutions[:5]:
            print(f"  {res} -> Good for {count}/{len(RETRO_SYSTEMS)} systems (avg: {avg:.1f}%)")

if __name__ == "__main__":
    main()