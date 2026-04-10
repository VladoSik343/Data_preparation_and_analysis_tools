import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, Button, CheckButtons
from scipy import signal
INIT_AMP = 1.5
INIT_FREQ = 2.3
INIT_PHASE = 1.2
INIT_NOISE_MEAN = 0.1
INIT_NOISE_COV = 0.15
INIT_CUTOFF = 25.5
fs = 800
t = np.linspace(0, 8, fs, endpoint=False)
base_noise = np.random.normal(0, 1, len(t))

def get_clean_harmonic(amp, freq, phase):
    return amp * np.sin(2 * np.pi * freq * t + phase)

def get_current_noise(mean, cov):
    return base_noise * np.sqrt(cov) + mean

def apply_filter(data, cutoff_freq):
    nyq = 0.5 * fs
    normal_cutoff = cutoff_freq / nyq
    if normal_cutoff <= 0 or normal_cutoff >= 1:
        return data
    b, a = signal.butter(4, normal_cutoff, btype='low')
    return signal.filtfilt(b, a, data)
fig, ax = plt.subplots(figsize=(12, 8))
plt.subplots_adjust(bottom=0.45, right=0.85)
clean_y = get_clean_harmonic(INIT_AMP, INIT_FREQ, INIT_PHASE)
noise_y = get_current_noise(INIT_NOISE_MEAN, INIT_NOISE_COV)
noisy_y = clean_y + noise_y
filtered_y = apply_filter(noisy_y, INIT_CUTOFF)
line_clean, = ax.plot(t, clean_y, lw=2, color='green', label='Чиста гармоніка')
line_noisy, = ax.plot(t, noisy_y, lw=1.5, color='orange', alpha=0.7, label='Зашумлений сигнал')
line_filtered, = ax.plot(t, filtered_y, lw=2, color='purple', linestyle='--', label='Відфільтрований сигнал')
ax.set_ylim(-4, 4)
ax.set_title("Аналіз гармонічного сигналу з шумом")
ax.set_xlabel("Час (с)")
ax.set_ylabel("Амплітуда")
ax.legend(loc='upper right')
ax.grid(True, linestyle='--', alpha=0.6)
ax_color = 'lightyellow'
ax_amp = plt.axes([0.15, 0.35, 0.55, 0.025], facecolor=ax_color)
ax_freq = plt.axes([0.15, 0.30, 0.55, 0.025], facecolor=ax_color)
ax_phase = plt.axes([0.15, 0.25, 0.55, 0.025], facecolor=ax_color)
ax_noise_mean = plt.axes([0.15, 0.15, 0.55, 0.025], facecolor=ax_color)
ax_noise_cov = plt.axes([0.15, 0.10, 0.55, 0.025], facecolor=ax_color)
ax_cutoff = plt.axes([0.15, 0.05, 0.55, 0.025], facecolor=ax_color)
sl_amp = Slider(ax_amp, 'Амплітуда (A)', 0.2, 3.5, valinit=INIT_AMP)
sl_freq = Slider(ax_freq, 'Частота (f, Гц)', 0.3, 12.0, valinit=INIT_FREQ)
sl_phase = Slider(ax_phase, 'Фаза (φ, рад)', 0.0, 2*np.pi, valinit=INIT_PHASE)
sl_noise_mean = Slider(ax_noise_mean, 'Середнє шуму (μ)', -0.8, 0.8, valinit=INIT_NOISE_MEAN)
sl_noise_cov = Slider(ax_noise_cov, 'Дисперсія шуму (σ²)', 0.0, 0.8, valinit=INIT_NOISE_COV)
sl_cutoff = Slider(ax_cutoff, 'Частота зрізу (Гц)', 5.0, 60.0, valinit=INIT_CUTOFF)
ax_check = plt.axes([0.75, 0.35, 0.18, 0.08], frame_on=True)
check = CheckButtons(ax_check, ['Показати шум'], [True])
ax_reset = plt.axes([0.75, 0.25, 0.18, 0.06])
btn_reset = Button(ax_reset, 'Скинути (Reset)', color='lightgreen', hovercolor='0.975')

instruct_text = (
    "Інструкція з користування:\n"
    "1)Слайдери змінюють параметри\n"
    "2)Чекбокс вмикає/вимикає шум\n"
    "3)Reset повертає початкові параметри\n"
    "4)Червона лінія - відфільтрований сигнал"
)
fig.text(0.75, 0.05, instruct_text, fontsize=9, verticalalignment='bottom',
         bbox=dict(boxstyle='square', facecolor='lightyellow', edgecolor='black'))

def update(val):
    c_y = get_clean_harmonic(sl_amp.val, sl_freq.val, sl_phase.val)
    n_y = get_current_noise(sl_noise_mean.val, sl_noise_cov.val)
    line_clean.set_ydata(c_y)
    line_noisy.set_ydata(c_y + n_y)
    line_filtered.set_ydata(apply_filter(c_y + n_y, sl_cutoff.val))
    fig.canvas.draw_idle()
sl_amp.on_changed(update)
sl_freq.on_changed(update)
sl_phase.on_changed(update)
sl_noise_mean.on_changed(update)
sl_noise_cov.on_changed(update)
sl_cutoff.on_changed(update)

def toggle_noise(label):
    line_noisy.set_visible(check.get_status()[0])
    fig.canvas.draw_idle()
check.on_clicked(toggle_noise)

def reset_all(event):
    sl_amp.reset()
    sl_freq.reset()
    sl_phase.reset()
    sl_noise_mean.reset()
    sl_noise_cov.reset()
    sl_cutoff.reset()
    if not check.get_status()[0]:
        check.set_active(0)
btn_reset.on_clicked(reset_all)
plt.show()