import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, Button, CheckButtons
from scipy.signal import butter, filtfilt

# 1. Параметри симуляції
# Створюємо часову вісь: від 0 до 10 секунд, 1000 точок для високої деталізації
t = np.linspace(0, 10, 1000)

# ФІКСАЦІЯ ШУМУ: Генеруємо базовий масив шуму один раз
# Це ключовий момент: при зміні амплітуди гармоніки ми будемо масштабувати цей масив,
# замість генерації нового. Це запобігає "стрибанню" графіка (jitter).
np.random.seed(42)
base_noise = np.random.normal(0, 1, len(t))

# Дефолтні значення параметрів для ініціалізації
INIT_AMP = 1.0
INIT_FREQ = 1.0
INIT_PHASE = 0.0
INIT_NOISE_MEAN = 0.0
INIT_NOISE_COV = 0.1
INIT_CUTOFF = 2.0


# 2. Математичний апарат
def harmonic_with_noise(t, amplitude, frequency, phase, noise_mean, noise_covariance, show_noise):
    """
    Генерує гармоніку y(t) = A*sin(ωt + φ) та додає гаусівський шум.
    """
    # Обчислення чистої синусоїди
    clean_signal = amplitude * np.sin(2 * np.pi * frequency * t + phase)

    # Накладання шуму: використовуємо базовий масив для стабільності візуалізації.
    # actual_noise = математичне очікування + середньоквадратичне відхилення * базовий шум
    actual_noise = noise_mean + np.sqrt(noise_covariance) * base_noise
    noisy_signal = clean_signal + actual_noise

    return (noisy_signal, clean_signal) if show_noise else (clean_signal, clean_signal)


def apply_filter(signal_data, cutoff_freq):
    """
    Цифрова фільтрація: використовуємо низькочастотний фільтр Баттерворта 3-го порядку.
    """
    # Частота Найквіста (половина частоти дискретизації)
    # На 10 сек у нас 1000 точок => частота дискретизації 100 Гц => Найквіст = 50 Гц
    nyquist = 50.0

    # Нормалізація частоти зрізу для функції butter (від 0 до 1)
    normal_cutoff = max(0.01, min(cutoff_freq / nyquist, 0.99))

    # Розрахунок коефіцієнтів фільтра (b, a) та застосування через filtfilt (нульовий фазовий зсув)
    b, a = butter(3, normal_cutoff, btype='low', analog=False)
    return filtfilt(b, a, signal_data)


# 3. Візуалізація та GUI
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))
plt.subplots_adjust(left=0.1, bottom=0.45, hspace=0.3)

# Налаштування верхнього графіка (Оригінал)
ax1.set_title("Вхідний сигнал: Гармоніка + Шум", fontsize=12, fontweight='bold')
ax1.set_xlim(0, 10);
ax1.set_ylim(-5, 5);
ax1.grid(True, linestyle='--', alpha=0.7)

# Налаштування нижнього графіка (Результат обробки)
ax2.set_title("Результат фільтрації (IIR Low-pass Filter)", fontsize=12, fontweight='bold')
ax2.set_xlim(0, 10);
ax2.set_ylim(-5, 5);
ax2.grid(True, linestyle='--', alpha=0.7)

# Перша отрисовка даних
noisy_y, clean_y = harmonic_with_noise(t, INIT_AMP, INIT_FREQ, INIT_PHASE, INIT_NOISE_MEAN, INIT_NOISE_COV, True)
filtered_y = apply_filter(noisy_y, INIT_CUTOFF)

line_noisy, = ax1.plot(t, noisy_y, lw=1, color='lightcoral', alpha=0.8, label='Зашумлений')
line_clean1, = ax1.plot(t, clean_y, lw=2, color='darkred', label='Чистий')
ax1.legend(loc='upper right')

line_filtered, = ax2.plot(t, filtered_y, lw=2, color='blue', label='Відфільтрований')
line_clean2, = ax2.plot(t, clean_y, lw=1.5, color='black', ls='--', alpha=0.5, label='Еталон')
ax2.legend(loc='upper right')

# 4. Елементи керування
axcolor = 'whitesmoke'

# Створення слайдерів для параметрів сигналу та шуму
ax_amp = plt.axes([0.15, 0.35, 0.65, 0.02], facecolor=axcolor)
ax_freq = plt.axes([0.15, 0.30, 0.65, 0.02], facecolor=axcolor)
ax_phase = plt.axes([0.15, 0.25, 0.65, 0.02], facecolor=axcolor)
ax_nmean = plt.axes([0.15, 0.20, 0.65, 0.02], facecolor=axcolor)
ax_ncov = plt.axes([0.15, 0.15, 0.65, 0.02], facecolor=axcolor)
ax_cutoff = plt.axes([0.15, 0.10, 0.65, 0.02], facecolor=axcolor)

s_amp = Slider(ax_amp, 'Амплітуда (A)', 0.1, 5.0, valinit=INIT_AMP)
s_freq = Slider(ax_freq, 'Частота (f)', 0.1, 10.0, valinit=INIT_FREQ)
s_phase = Slider(ax_phase, 'Фаза (φ)', 0.0, 2 * np.pi, valinit=INIT_PHASE)
s_nmean = Slider(ax_nmean, 'Сер. шум', -2.0, 2.0, valinit=INIT_NOISE_MEAN)
s_ncov = Slider(ax_ncov, 'Дисперсія', 0.0, 2.0, valinit=INIT_NOISE_COV)
s_cutoff = Slider(ax_cutoff, 'Зріз фільтра', 0.1, 20.0, valinit=INIT_CUTOFF)

# Чекбокс відображення шуму
ax_check = plt.axes([0.85, 0.35, 0.1, 0.05])
cb_noise = CheckButtons(ax_check, ['Шум'], [True])

# Кнопка скидання параметрів
ax_reset = plt.axes([0.85, 0.25, 0.1, 0.05])
btn_reset = Button(ax_reset, 'Reset', hovercolor='0.95')


# 5. Обробка подій (Інтерактивність)
def update(val):
    # Зчитуємо поточні значення зі слайдерів
    amp, freq, ph = s_amp.val, s_freq.val, s_phase.val
    n_mean, n_cov, cutoff = s_nmean.val, s_ncov.val, s_cutoff.val
    show_n = cb_noise.get_status()[0]

    # Оновлюємо дані для обох графіків
    noisy_val, clean_val = harmonic_with_noise(t, amp, freq, ph, n_mean, n_cov, show_n)

    # Фільтруємо завжди зашумлений сигнал (навіть якщо він прихований), щоб бачити роботу фільтра
    _, real_noisy_data = harmonic_with_noise(t, amp, freq, ph, n_mean, n_cov, True)
    filt_val = apply_filter(real_noisy_data, cutoff)

    # Оновлення об'єктів ліній на графіку (оптимізовано для швидкості)
    line_noisy.set_ydata(noisy_val)
    line_noisy.set_visible(show_n)
    line_clean1.set_ydata(clean_val)
    line_clean2.set_ydata(clean_val)
    line_filtered.set_ydata(filt_val)

    fig.canvas.draw_idle()


# Реєстрація функцій оновлення
for s in [s_amp, s_freq, s_phase, s_nmean, s_ncov, s_cutoff]:
    s.on_changed(update)
cb_noise.on_clicked(update)


def reset(event):
    """Скидає всі віджети до початкових значень."""
    for s in [s_amp, s_freq, s_phase, s_nmean, s_ncov, s_cutoff]:
        s.reset()
    if not cb_noise.get_status()[0]:
        cb_noise.set_active(0)


btn_reset.on_clicked(reset)

plt.show()