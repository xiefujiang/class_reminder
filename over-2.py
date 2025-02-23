import re
from collections import defaultdict


def copy_file_with_replacements_and_expand(input_file, output_ini_file):
    # 打开输入文件并读取内容
    with open(input_file, 'r', encoding='utf-8') as infile:
        lines = infile.readlines()

    # 使用正则表达式删除所有的“第几周”字样（针对每行处理）
    import re
    lines_without_weeks = [re.sub(r'第\d+周', '', line) for line in lines]

    # 替换星期几的中文名称为英文名称（针对每行处理）
    day_replacements = {
        '星期一': 'Monday',
        '星期二': 'Tuesday',
        '星期三': 'Wednesday',
        '星期四': 'Thursday',
        '星期五': 'Friday',
        '星期六': 'Saturday',
        '星期日': 'Sunday'
    }

    # 替换特定的节名称为cls1, cls2,..., cls5（针对每行处理）
    section_replacements = {
        '[01-02]节': 'cls1',
        '[03-04]节': 'cls2',
        '[05-06]节': 'cls3',
        '[07-08]节': 'cls4',
        '[09-10]节': 'cls5'
    }

    # 对每行进行替换处理
    processed_lines = []
    for line in lines_without_weeks:
        # 替换星期几和节名称
        for chinese_day, english_day in day_replacements.items():
            line = line.replace(chinese_day, english_day)
        for old_section, new_section in section_replacements.items():
            line = line.replace(old_section, new_section)
            # 替换上课时间（注意这里是最终的替换，冒号位置已修正）
        line = line.replace("上课时间： ", "上课时间:")
        processed_lines.append(line)

    # 将每三行扩增为四行，新增一行空行在第一行
    expanded_lines = []
    for i in range(0, len(processed_lines), 3):
        # 添加空行
        expanded_lines.append('\n')
        # 添加原来的三行
        expanded_lines.extend(processed_lines[i:i + 3])

    # 如果最后一组不足三行，则直接添加剩余的行
    if len(processed_lines) % 3!= 0:
        expanded_lines.extend(processed_lines[-(len(processed_lines) % 3):])

    # 准备进行最终的遍历和修改
    final_lines = expanded_lines[:]  # 复制一份expanded_lines，避免直接修改原列表

    # 定义一个正则表达式来匹配“上课时间:“后面的第一个单词
    import re
    class_time_pattern = re.compile(r'上课时间:(\s+\w+)')

    # 遍历final_lines，找到“上课时间:“并处理
    for i in range(len(final_lines)):
        # 检查当前行是否包含“上课时间:“
        if '上课时间:' in final_lines[i]:
            # 使用正则表达式查找第一个单词
            match = class_time_pattern.search(final_lines[i])
            if match:
                # 获取第一个单词（去掉前导空格）
                first_word = match.group(1).strip()
                # 计算-2行的索引（注意考虑空行和边界情况）
                index_minus_2 = i - 2
                # 确保-2行存在且不是空行
                if index_minus_2 >= 0 and final_lines[index_minus_2].strip():
                    # 将第一个单词添加到-2行的末尾（这里假设用空格分隔）
                    final_lines[index_minus_2] = final_lines[index_minus_2].rstrip() + ' ' + first_word

    # 将处理后的内容写入输出INI文件
    with open(output_ini_file, 'w', encoding='utf-8') as outfile:
        outfile.writelines(final_lines)


def process_file(input_file, output_file):
    lines = []
    with open(input_file, 'r', encoding='utf-8') as file:  # 指定以UTF-8编码读取
        lines = file.readlines()

    processed_lines = []  # 用来存储处理后的行

    for i in range(0, len(lines), 4):
        # 确保有足够的行处理
        if i + 3 >= len(lines):
            # 如果剩余行数不足四行，则将剩余行直接添加到处理后的行列表中
            processed_lines.extend(lines[i:])
            break

        # 获取每四行的内容
        first_line = lines[i]
        second_line = lines[i + 1]
        third_line = lines[i + 2]
        fourth_line = lines[i + 3]

        # 查找第三行中的星期几（假设星期几是唯一的且格式正确）
        week_days = ["Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"]
        day_found = None
        for day in week_days:
            if day in third_line:
                day_found = day
                break

        # 如果找到了星期几，则进行剪切并添加到第一行，同时加上[]
        if day_found:
            first_line = first_line.rstrip() + f" [{day_found}]" + "\n"
            # 移除第三行中的星期几（这里假设星期几后面可能有空格或其他字符，因此使用replace）
            third_line = third_line.replace(day_found, "").strip() + "\n"

        # 替换“上课时间: ”为“上课时间:”（移除空格）和全角冒号为半角冒号
        first_line = first_line.replace("上课时间: ", "上课时间:").replace("：", ":").replace(" [", "[")
        second_line = second_line.replace("上课时间: ", "上课时间:").replace("：", ":").replace(" [", "[")
        third_line = third_line.replace("上课时间: ", "上课时间:").replace("：", ":").replace(" [", "[")
        fourth_line = fourth_line.replace("上课时间: ", "上课时间:").replace("：", ":").replace(" [", "[")

        # 将处理后的行添加到结果列表中
        processed_lines.append(first_line)
        processed_lines.append(second_line)
        processed_lines.append(third_line)
        processed_lines.append(fourth_line)

    # 将处理后的内容写入到输出文件
    with open(output_file, 'w', encoding='utf-8') as file:  # 指定以UTF-8编码写入
        file.writelines(processed_lines)


def read_and_modify_groups_from_ini(input_file_path, output_file_path):
    try:
        # 打开输入文件并读取所有行
        with open(input_file_path, 'r', encoding='utf-8') as input_file:
            lines = input_file.readlines()

        # 准备一个列表来存储修改后的行
        modified_lines = []

        # 遍历行，每次读取四行作为一组
        for i in range(0, len(lines), 4):
            # 获取当前组的四行（或者剩余的行，如果不足四行则警告并跳过）
            group = lines[i:i + 4]
            if len(group) < 4:
                print(f"警告: 在文件 {input_file_path} 中找到一个不完整的组，只有 {len(group)} 行，跳过该组。")
                continue

            # 解析每行的内容
            day_line = group[0].strip()  # 第一行通常是日期，我们这里不修改它
            course_name_line = group[1].strip()
            class_time_line = group[2].strip()
            location_line = group[3].strip()

            # 提取课程名称、上课时间和上课地点
            course_name = extract_value_after_key(course_name_line, "课程名称:")
            class_time = extract_value_after_key(class_time_line, "上课时间:")
            location = extract_value_after_key(location_line, "上课地点:")

            # 修改第二、三、四行
            modified_group = [
                day_line + '\n',  # 保留第一行不变，添加换行符
                f"{class_time}_name:{course_name}\n",  # 修改第二行
                f"{class_time}_teacher:none\n",  # 修改第三行
                f"{class_time}_clsrm:{location}\n"  # 修改第四行
            ]

            # 将修改后的组添加到修改后的行列表中
            modified_lines.extend(modified_group)

        # 将修改后的内容写入输出文件
        with open(output_file_path, 'w', encoding='utf-8') as output_file:
            output_file.writelines(modified_lines)

        print(f"修改后的内容已写入文件 {output_file_path}")

    except Exception as e:
        print(f"读取或写入文件时发生错误: {e}")


def extract_value_after_key(line, key):
    try:
        key_index = line.index(key) + len(key)
        value = line[key_index:].strip()
        return value
    except ValueError:
        # 如果键不存在于行中，返回一个空字符串
        return ""


def read_and_integrate_groups_by_week(input_file_path, output_file_path):
    try:
        # 打开输入文件并读取所有行
        with open(input_file_path, 'r', encoding='utf-8') as input_file:
            lines = input_file.readlines()

            # 使用一个字典来按照星期整合组
        integrated_groups = defaultdict(list)

        # 遍历行，每次读取四行作为一组
        for i in range(0, len(lines), 4):
            # 获取当前组的四行
            group = lines[i:i + 4]
            if len(group) < 4:
                print(f"警告: 在文件 {input_file_path} 中找到一个不完整的组，只有 {len(group)} 行，跳过该组。")
                continue

                # 第一行是星期，我们用它作为键来整合组
            week_line = group[0].strip()
            # 提取星期的具体值（这里假设星期行是以"星期"开头的，后面跟着具体的星期几）
            week_value = week_line.split(":", 1)[-1].strip() if ":" in week_line else week_line

            # 将第二、第三和第四行添加到对应星期的列表中
            integrated_groups[week_value].extend(group[1:4])

            # 准备一个列表来存储整合后的行
        integrated_lines = []

        # 遍历整合后的组，为每个星期添加一个标题，并添加其对应的行
        for week, group_lines in integrated_groups.items():
            integrated_lines.append(f"{week}\n")  # 添加星期标题
            integrated_lines.extend(group_lines)  # 添加该星期的所有组行

        # 将整合后的内容写入输出文件
        with open(output_file_path, 'w', encoding='utf-8') as output_file:
            output_file.writelines(integrated_lines)

        print(f"整合后的内容已写入文件 {output_file_path}")

    except Exception as e:
        print(f"读取或写入文件时发生错误: {e}")


def add_empty_lines_before_brackets_and_replace_colons(file_path):
    # 读取文件内容
    with open(file_path, 'r', encoding='utf-8') as file:
        lines = file.readlines()

        # 用于存储修改后的文件内容
    modified_lines = []

    # 遍历文件内容，检查每一行
    for i in range(len(lines)):
        # 检查当前行的下一行是否存在，并且是否包含 "["
        if i + 1 < len(lines) and "[" in lines[i + 1] and not lines[i + 1].strip().startswith('['):  # 确保不是已经是部分开头的行
            # 在当前行后面添加一个空行（即，在 "[" 行上面添加一个空行）
            modified_lines.append(lines[i])
            modified_lines.append("\n")  # 添加一个空行
        else:
            # 否则，直接添加当前行，但替换所有的 ":" 为 "="
            modified_line = lines[i].replace(':', '=')
            modified_lines.append(modified_line)

            # 将修改后的内容写回文件
    with open(file_path, 'w', encoding='utf-8') as file:
        file.writelines(modified_lines)


def read_section(file, section_tag):
    section_lines = []
    current_section = None
    in_target_section = False
    with open(file, 'r', encoding='utf-8') as f:
        for line in f:
            stripped_line = line.strip()
            if stripped_line.startswith('[') and stripped_line.endswith(']'):
                current_section = stripped_line
                in_target_section = (current_section == section_tag)
            elif in_target_section and '=' in stripped_line:
                section_lines.append(line)
            elif current_section and current_section!= section_tag:
                in_target_section = False
    return section_lines


def write_file_with_replacements(file, sections_and_replacements):
    output_lines = []
    current_section = None
    with open(file, 'r', encoding='utf-8') as f:
        for line in f:
            stripped_line = line.strip()
            if stripped_line.startswith('[') and stripped_line.endswith(']'):
                current_section = stripped_line
            elif current_section and '=' in stripped_line:
                key, value = stripped_line.split('=', 1)
                key = key.strip()
                if current_section in sections_and_replacements and key in sections_and_replacements[current_section]:
                    value = sections_and_replacements[current_section][key]
                    line = f"{key}={value}\n"
            output_lines.append(line)
    with open(file, 'w', encoding='utf-8') as f:
        f.writelines(output_lines)


def main():
    input_file = 'kebiao.txt'  # 输入文件路径，请根据实际情况修改
    first_output_file = 'output.ini'  # 第一个中间输出INI文件路径，可按需修改
    final_output_file = 'output.ini'  # 最终输出文件路径，这里示例中与中间文件同名，可按需调整

    # 第一步，执行copy_file_with_replacements_and_expand函数
    copy_file_with_replacements_and_expand(input_file, first_output_file)
    # 第二步，执行process_file函数
    process_file(first_output_file, first_output_file)
    # 第三步，执行read_and_modify_groups_from_ini函数
    read_and_modify_groups_from_ini(first_output_file, first_output_file)
    # 第四步，执行read_and_integrate_groups_by_week函数
    read_and_integrate_groups_by_week(first_output_file, first_output_file)
    # 第五步，执行add_empty_lines_before_brackets_and_replace_colons函数
    add_empty_lines_before_brackets_and_replace_colons(first_output_file)
    # 第六步，执行相关的配置文件替换操作（这里假设原逻辑中的相关处理依然适用）
    all_replacements = {}
    for day in range(1, 6):
        section_tag = f'[{["Monday", "Tuesday", "Wednesday", "Thursday", "Friday"][day - 1]}]'
        file1_section = read_section(first_output_file, section_tag)
        replacements = {line.split('=')[0].strip(): line.split('=')[1].strip() for line in file1_section}
        all_replacements[section_tag] = replacements
    write_file_with_replacements('Config/cfg.ini', all_replacements)


if __name__ == '__main__':
    main()