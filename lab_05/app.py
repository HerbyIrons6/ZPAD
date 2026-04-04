import streamlit as st
import pandas as pd
import plotly.express as px
import numpy as np

# Налаштування базових параметрів сторінки.
# layout="wide" дозволяє використовувати всю ширину екрану, що зручно для великих таблиць та графіків.
st.set_page_config(page_title="Дашборд даних NOAA", layout="wide")

# 1. ГЕНЕРАЦІЯ ДАНИХ
# Оскільки ми не використовуємо зовнішні файли, створюємо функцію для генерації синтетичного датасету.
def generate_data():
    # np.repeat дублює кожен рік (52 тижні * 5 областей), створюючи базову вісь часу
    years = np.repeat(np.arange(2000, 2024), 52 * 5)

    # np.tile повторює послідовність від 1 до 52 для кожного року
    weeks = np.tile(np.repeat(np.arange(1, 53), 5), 24)

    # Створюємо масив областей і розмножуємо його на всі тижні та роки
    regions = np.tile(np.array(["Вінницька", "Київська", "Одеська", "Львівська", "Харківська"]), 24 * 52)

    # Генеруємо випадкові значення вегетаційних індексів у діапазоні від 10 до 90
    # Використовуємо рівномірний розподіл (uniform)
    vci = np.random.uniform(10, 90, len(years))
    tci = np.random.uniform(10, 90, len(years))
    vhi = np.random.uniform(10, 90, len(years))

    # Збираємо всі масиви в єдиний DataFrame для зручної аналітики
    return pd.DataFrame({"Year": years, "Week": weeks, "Region": regions, "VCI": vci, "TCI": tci, "VHI": vhi})

# 2. КЕРУВАННЯ СТАНОМ СЕСІЇ
# Streamlit перезапускає весь скрипт при кожному кліку.
# Щоб наші випадкові дані не змінювалися щоразу, коли ми тягнемо повзунок,
# ми зберігаємо їх у st.session_state (глобальний словник сесії користувача).
if 'df' not in st.session_state:
    st.session_state.df = generate_data()

# Завантажуємо дані з пам'яті сесії в робочу змінну
df = st.session_state.df

# 3. ІНІЦІАЛІЗАЦІЯ ТА СКИДАННЯ ФІЛЬТРІВ
def reset_filters():
    """
    Функція callback. Призначає ключам віджетів у session_state їхні дефолтні значення.
    Це дозволяє миттєво повернути інтерфейс до початкового стану.
    """
    st.session_state.index_sel = "VHI"
    st.session_state.region_sel = df["Region"].unique()[0]
    st.session_state.week_range = (1, 52)
    st.session_state.year_range = (int(df["Year"].min()), int(df["Year"].max()))
    st.session_state.sort_asc = False
    st.session_state.sort_desc = False

# Якщо програма запускається вперше, ініціалізуємо параметри за замовчуванням
if 'index_sel' not in st.session_state:
    reset_filters()

# 4. UI
# Розділяємо екран на дві колонки: вузьку для керування (відношення 1) і широку для візуалізації (відношення 3)
col1, col2 = st.columns([1, 3])

with col1:
    st.header("⚙️ Параметри")

    # Кнопка для ручного оновлення датасету.
    # Вона перезаписує дані в сесії та примусово оновлює сторінку (st.rerun).
    if st.button("🎲 Згенерувати нові дані", use_container_width=True):
        st.session_state.df = generate_data()
        st.rerun()

    st.markdown("---")

    # Віджети керування. Параметр 'key' автоматично зв'язує їх зі змінними у st.session_state
    index = st.selectbox("Індекс", ["VCI", "TCI", "VHI"], key='index_sel')
    region = st.selectbox("Область", df["Region"].unique(), key='region_sel')
    weeks = st.slider("Інтервал тижнів", 1, 52, key='week_range')
    years = st.slider("Інтервал років", int(df["Year"].min()), int(df["Year"].max()), key='year_range')

    st.markdown("### Сортування")
    sort_asc = st.checkbox("За зростанням", key='sort_asc')
    sort_desc = st.checkbox("За спаданням", key='sort_desc')

    # Кнопка Reset викликає функцію reset_filters (callback) ПЕРЕД наступним перемальовуванням
    st.button("🔄 Reset (Скинути фільтри)", on_click=reset_filters, use_container_width=True)

# 5. ЛОГІКА ФІЛЬТРАЦІЇ ДАНИХ
# Створюємо булеву маску для фільтрації Pandas DataFrame за всіма обраними критеріями одночасно
mask = (
        (df["Region"] == region) &
        (df["Week"] >= weeks[0]) & (df["Week"] <= weeks[1]) &
        (df["Year"] >= years[0]) & (df["Year"] <= years[1])
)
# Робимо копію, щоб не пошкодити оригінальний датасет у пам'яті
df_filtered = df[mask].copy()

# Обробка логіки сортування з урахуванням "дурня" (якщо юзер вибрав обидва варіанти)
with col2:
    if sort_asc and sort_desc:
        st.warning("⚠️ Увага: обрано обидва чекбокси сортування! Сортування скасовано (дані показані хронологічно).")
        df_filtered = df_filtered.sort_values(by=["Year", "Week"])
    elif sort_asc:
        df_filtered = df_filtered.sort_values(by=index, ascending=True)
    elif sort_desc:
        df_filtered = df_filtered.sort_values(by=index, ascending=False)
    else:
        # Дефолтний стан — хронологічний порядок
        df_filtered = df_filtered.sort_values(by=["Year", "Week"])

#  6. ВІДОБРАЖЕННЯ РЕЗУЛЬТАТІВ
with col2:
    st.title("Аналіз вегетаційних індексів")

    # Створюємо вкладки для зручної організації контенту (щоб не скролити вниз)
    tab1, tab2, tab3 = st.tabs(["🗃 Дані (Таблиця)", "📈 Динаміка індексу", "📊 Порівняння областей"])

    with tab1:
        st.subheader("Відфільтровані дані")
        # st.dataframe оптимізовано для відображення великих масивів даних
        st.dataframe(df_filtered, use_container_width=True, height=400)

    with tab2:
        st.subheader(f"Часовий ряд {index} для області: {region}")
        # Для графіка обов'язково сортуємо за часом, незалежно від налаштувань чекбоксів
        df_plot = df_filtered.copy().sort_values(by=["Year", "Week"])
        # Створюємо зручну мітку для осі Х, об'єднуючи рік і тиждень
        df_plot["Date"] = df_plot["Year"].astype(str) + " - Тиждень " + df_plot["Week"].astype(str)

        # Побудова інтерактивного лінійного графіка за допомогою Plotly
        fig1 = px.line(df_plot, x="Date", y=index, markers=True,
                       title=f"Зміна {index} з {years[0]} по {years[1]} рік")
        st.plotly_chart(fig1, use_container_width=True)

    with tab3:
        st.subheader(f"Порівняння {index} між усіма областями")

        # Для порівняння нам потрібні дані по ВСІХ областях, але тільки за обраний проміжок часу
        mask_all_regions = (
                (df["Week"] >= weeks[0]) & (df["Week"] <= weeks[1]) &
                (df["Year"] >= years[0]) & (df["Year"] <= years[1])
        )

        # Групуємо відфільтровані дані за регіонами та рахуємо середнє значення обраного індексу
        df_compare = df[mask_all_regions].groupby("Region")[index].mean().reset_index()

        # Додаємо допоміжну колонку для розмальовки графіка (щоб виділити обрану зліва область)
        df_compare["Колір"] = df_compare["Region"].apply(lambda x: "Обрана область" if x == region else "Інші області")

        # Побудова стовпчикової діаграми
        fig2 = px.bar(df_compare, x="Region", y=index, color="Колір",
                      color_discrete_map={"Обрана область": "#e74c3c", "Інші області": "#3498db"},
                      title=f"Середнє значення {index} за вказаний період")
        st.plotly_chart(fig2, use_container_width=True)