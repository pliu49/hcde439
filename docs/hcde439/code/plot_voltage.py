"""Plot V_measure (volts) against time from a serial monitor log of sunroom_fan.ino.

Usage:
    python3 plot_voltage.py serial_log.txt ../images/a3_voltage_vs_time.png [t0_ms]

Each log line looks like:
    Sunroom fan | t=5230 ms | A0=512 | V=2.50 V | PWM=156 | fan ON
t0_ms (optional) is the Arduino time that should be drawn as 0 s on the x-axis;
by default the first sample is 0 s.
"""
import re
import sys

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt

LINE = re.compile(r"t=(\d+) ms \| A0=(\d+) \| V=([\d.]+) V \| PWM=(\d+)")
DARK_ADC = 250                       # DARK_THRESHOLD in the firmware
DARK_V = DARK_ADC * 5.0 / 1023.0     # the same threshold in volts (about 1.22 V)

SURFACE, INK, INK_2, MUTED, GRID, SERIES = "#fcfcfb", "#0b0b0b", "#52514e", "#8a8984", "#e6e5e1", "#2a78d6"

log_path, out_path = sys.argv[1], sys.argv[2]
t0_arg = float(sys.argv[3]) if len(sys.argv) > 3 else None

samples = []
with open(log_path) as log:
    for line in log:
        match = LINE.search(line)
        if match:
            samples.append((int(match.group(1)), int(match.group(2)), float(match.group(3)), int(match.group(4))))
if not samples:
    sys.exit("No matching lines found; check that the log has the format shown above.")

t0 = t0_arg if t0_arg is not None else samples[0][0]
times = [(t - t0) / 1000.0 for t, _, _, _ in samples]
volts = [v for _, _, v, _ in samples]
adc = [a for _, a, _, _ in samples]

fig, ax = plt.subplots(figsize=(9, 4.8), facecolor=SURFACE)
ax.set_facecolor(SURFACE)
ax.plot(times, volts, color=SERIES, linewidth=2, marker="o", markersize=6,
        markeredgecolor=SURFACE, markeredgewidth=1.5, zorder=3)

# reference line: the "too dark" threshold used by the if-statement
ax.axhline(DARK_V, color=MUTED, linestyle=(0, (4, 3)), linewidth=1.2, zorder=1)
ax.text(times[0], DARK_V + 0.1, f"dark threshold: ADC {DARK_ADC} = {DARK_V:.2f} V (fan off below)",
        ha="left", va="bottom", fontsize=9, color=INK_2)

# first sample at or above the threshold = fan switches on
on = next((i for i, a in enumerate(adc) if a >= DARK_ADC), None)
if on is not None and on > 0:
    ax.axvline(times[on], color=MUTED, linewidth=1, linestyle=":", zorder=1)
    ax.annotate(f"fan switches on\n({volts[on]:.2f} V, ADC {adc[on]})", xy=(times[on], volts[on]),
                xytext=(times[on] + 0.3, 3.9), fontsize=9, color=INK_2,
                arrowprops=dict(arrowstyle="-", color=MUTED, linewidth=1))
    ax.text((times[0] + times[on]) / 2, 0.25, "fan OFF (dark)", ha="center", fontsize=9, color=INK_2)
    ax.text((times[on] + times[-1]) / 2 + 0.3, 0.25, f"fan ON, V settles near {volts[-1]:.2f} V",
            ha="center", fontsize=9, color=INK_2)

ax.set_xlabel("Time in demo video (s)", color=INK_2)
ax.set_ylabel("Voltage at V_measure, pin A0 (V)", color=INK_2)
ax.set_ylim(0, 5)
ax.set_xlim(min(times) - 0.1, max(times) + 0.1)
ax.set_title("Divider voltage while the light on the photoresistor changes", loc="left", fontsize=12, color=INK)
ax.grid(axis="y", color=GRID, linewidth=1)
ax.tick_params(colors=INK_2, length=0)
for side in ("top", "right", "left"):
    ax.spines[side].set_visible(False)
ax.spines["bottom"].set_color(GRID)
fig.tight_layout()
fig.savefig(out_path, dpi=150, facecolor=SURFACE)
print(f"Saved {out_path} with {len(times)} samples")
