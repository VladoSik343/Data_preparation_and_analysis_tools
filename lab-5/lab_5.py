import streamlit as st
import pandas as pd
import matplotlib.pyplot as plt
import urllib.request
import os
from datetime import datetime
st.title("Аналіз VCI/TCI/VHI")
os.makedirs("VHI_data", exist_ok=True)

def download_all_files():
    with st.spinner("Завантаження даних з NOAA..."):
        for provinceID in range(1, 28):
            url = f"https://www.star.nesdis.noaa.gov/smcd/emb/vci/VH/get_TS_admin.php?country=UKR&provinceID={provinceID}&year1=1981&year2=2024&type=Mean"
            time = datetime.now().strftime("%Y%m%d")
            file_name = f"vhi_province_{provinceID}_{time}.csv"
            file_path = os.path.join("VHI_data", file_name)
            if not os.path.exists(file_path):
                urllib.request.urlretrieve(url, file_path)
    st.success("Всі файли завантажено!")
if not os.listdir("VHI_data"):
    download_all_files()
province_names = {
    1: "Черкаська", 2: "Чернігівська", 3: "Чернівецька", 4: "Республіка Крим",
    5: "Дніпропетровська", 6: "Донецька", 7: "Івано-Франківська", 8: "Харківська",
    9: "Херсонська", 10: "Хмельницька", 11: "Київська", 12: "Київ",
    13: "Кіровоградська", 14: "Луганська", 15: "Львівська", 16: "Миколаївська",
    17: "Одеська", 18: "Полтавська", 19: "Рівненська", 20: "Севастополь",
    21: "Сумська", 22: "Тернопільська", 23: "Закарпатська", 24: "Вінницька",
    25: "Волинська", 26: "Запорізька", 27: "Житомирська"
}
prv_list = list(province_names.values())
min_y, max_y = 1982, 2024

def week_to_date(year, week):
    date_str = f"{year}-W{week}-1"
    return pd.to_datetime(date_str, format='%G-W%V-%u', errors='coerce')

@st.cache_data
def load_data(province_num):
    time = datetime.now().strftime("%Y%m%d")
    file_path = f'VHI_data/vhi_province_{province_num}_{time}.csv'
    if not os.path.exists(file_path):
        for f in os.listdir("VHI_data"):
            if f.startswith(f"vhi_province_{province_num}_"):
                file_path = os.path.join("VHI_data", f)
                break
    df = pd.read_csv(file_path, skiprows=2, encoding='utf-8', usecols=range(7))
    df.columns = ['year', 'week', 'SMN', 'SMT', 'VCI', 'TCI', 'VHI']
    df = df.replace(r'<[^>]*>', '', regex=True)
    for col in df.columns:
        df[col] = pd.to_numeric(df[col], errors='coerce')
    return df.dropna()

def filter_data(df, year_range, week_range):
    return df[(df['year'] >= year_range[0]) & 
              (df['year'] <= year_range[1]) & 
              (df['week'] >= week_range[0]) & 
              (df['week'] <= week_range[1])]

if 'idx' not in st.session_state: st.session_state.idx = "VCI"
if 'prv' not in st.session_state: st.session_state.prv = prv_list[0]
if 'wks' not in st.session_state: st.session_state.wks = (1, 52)
if 'yrs' not in st.session_state: st.session_state.yrs = (min_y, max_y)
if 'sort_asc' not in st.session_state: st.session_state.sort_asc = False
if 'sort_desc' not in st.session_state: st.session_state.sort_desc = False

def get_index(list_name, value):
    return list_name.index(value) if value in list_name else 0
left_col, right_col = st.columns([1, 2], gap="large")
with left_col:
    st.subheader("Фільтри та налаштування")
    if st.button('Скинути всі фільтри', use_container_width=True):
        st.session_state.idx = "VCI"
        st.session_state.prv = prv_list[0]
        st.session_state.wks = (1, 52)
        st.session_state.yrs = (min_y, max_y)
        st.session_state.sort_asc = False
        st.session_state.sort_desc = False
        st.rerun()
    st.divider()
    st.selectbox('Виберіть область:', prv_list, index=get_index(prv_list, st.session_state.prv), key='prv')
    st.selectbox('Виберіть часовий ряд:', ['VCI', 'TCI', 'VHI'], index=['VCI', 'TCI', 'VHI'].index(st.session_state.idx), key='idx')
    st.slider('Інтервал тижнів:', 1, 52, st.session_state.wks, key='wks')
    st.slider('Інтервал років:', min_y, max_y, st.session_state.yrs, 1, key='yrs')
    st.divider()
    st.write("### Сортування даних")
    col_asc, col_desc = st.columns(2)
    with col_asc:
        sort_asc = st.checkbox('За зростанням', key='sort_asc')
    with col_desc:
        sort_desc = st.checkbox('За спаданням', key='sort_desc')
    if sort_asc and sort_desc:
        st.error("Неможливо сортувати одночасно!")
        st.warning("Сортування вимкнено")
        sort_order = None
    elif sort_asc:
        st.success("Відсортовано за зростанням")
        sort_order = 'ascending'
    elif sort_desc:
        st.success("Відсортовано за спаданням")
        sort_order = 'descending'
    else:
        sort_order = None

with right_col:
    province_num = [k for k, v in province_names.items() if v == st.session_state.prv][0]
    df = load_data(province_num)
    df_filtered = filter_data(df, st.session_state.yrs, st.session_state.wks)
    df_filtered = df_filtered.copy()
    df_filtered['date'] = df_filtered.apply(lambda x: week_to_date(int(x['year']), int(x['week'])), axis=1)
    if sort_order == 'ascending':
        df_sorted = df_filtered.sort_values(by=st.session_state.idx, ascending=True)
    elif sort_order == 'descending':
        df_sorted = df_filtered.sort_values(by=st.session_state.idx, ascending=False)
    else:
        df_sorted = df_filtered
    tab1, tab2 = st.tabs(["Таблиця даних", "Графік та діаграма"])
    with tab1:
        st.write(f"### Таблиця даних для {st.session_state.prv} області")
        st.caption(f"**{st.session_state.idx}** | Роки: {st.session_state.yrs[0]}-{st.session_state.yrs[1]} | Тижні: {st.session_state.wks[0]}-{st.session_state.wks[1]}")
        show_df = df_sorted[['year', 'week', st.session_state.idx]].copy()
        show_df.columns = ['Рік', 'Тиждень', st.session_state.idx]
        st.dataframe(show_df, use_container_width=True)
    with tab2:
        st.write("### Візуалізація даних")
        st.write(f"#### Динаміка {st.session_state.idx} для {st.session_state.prv} області")
        df_for_plot = df_filtered.sort_values('date')
        fig1, ax1 = plt.subplots(figsize=(10, 4))
        ax1.plot(df_for_plot['date'], df_for_plot[st.session_state.idx], 
                marker='o', linestyle='-', linewidth=2, markersize=3, color='blue')
        ax1.set_xlabel('Дата')
        ax1.set_ylabel(st.session_state.idx)
        ax1.set_title(f"{st.session_state.idx} - {st.session_state.prv} область")
        ax1.grid(True, alpha=0.3)
        plt.xticks(rotation=45)
        st.pyplot(fig1)
        col1, col2, col3 = st.columns(3)
        with col1:
            st.metric("Мінімум", f"{df_filtered[st.session_state.idx].min():.2f}")
        with col2:
            st.metric("Максимум", f"{df_filtered[st.session_state.idx].max():.2f}")
        with col3:
            st.metric("Середнє", f"{df_filtered[st.session_state.idx].mean():.2f}")
        st.divider()
        
        st.write(f"#### Порівняння {st.session_state.idx} по областях")
        comparison_type = st.radio(
            "Режим порівняння:",
            ["Всі області", "ТОП області + обрана"],
            horizontal=True
        )
        
        means_data = []
        for prov_name in prv_list:
            prov_num_temp = [k for k, v in province_names.items() if v == prov_name][0]
            df_temp = load_data(prov_num_temp)
            df_temp_filtered = filter_data(df_temp, st.session_state.yrs, st.session_state.wks)
            means_data.append({
                'Область': prov_name,
                'Середнє': df_temp_filtered[st.session_state.idx].mean()
            })
        
        means_df = pd.DataFrame(means_data).sort_values('Середнє', ascending=False)
        
        if comparison_type == "ТОП області + обрана":
            top_n = st.slider("Кількість областей у ТОП:", 3, 27, 10)
            top_df = means_df.head(top_n).copy()
            if st.session_state.prv not in top_df['Область'].values:
                selected_row = means_df[means_df['Область'] == st.session_state.prv]
                top_df = pd.concat([top_df, selected_row], ignore_index=True)
            display_df = top_df
        else:
            display_df = means_df
        
        fig2, ax2 = plt.subplots(figsize=(12, 6))
        colors = ['red' if x == st.session_state.prv else 'steelblue' for x in display_df['Область']]
        bars = ax2.bar(display_df['Область'], display_df['Середнє'], color=colors, edgecolor='navy')
        ax2.set_xlabel('Область')
        ax2.set_ylabel(f'Середнє значення {st.session_state.idx}')
        ax2.set_title(f"Порівняння середніх значень {st.session_state.idx} по областях")
        ax2.grid(True, alpha=0.3, axis='y')
        plt.xticks(rotation=90)
        
        for bar, val in zip(bars, display_df['Середнє']):
            ax2.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.5, f'{val:.1f}', 
                    ha='center', va='bottom', fontsize=8)
        
        st.pyplot(fig2)
        st.success(f"**{st.session_state.prv} область** виділена червоним кольором")
        st.dataframe(display_df, use_container_width=True)
