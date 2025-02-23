import configparser
import os

# 首先进行课表文件中替换老师名字的相关操作

# 读取课表文件cfg.ini
config = configparser.ConfigParser()
config.read('Config/cfg.ini', encoding='utf-8')

# 读取老师表单文件laoshi.txt，将课程和老师对应存入字典
teacher_dict = {}
with open('laoshi.txt', 'r', encoding='utf-8') as f:
    # 逐行读取文件内容
    for line in f.readlines():
        # 去除每行两边的空白字符（如空格、换行符等），然后按空格分割出课程名和老师名
        course, teacher = line.strip().split(' ')
        teacher_dict[course] = teacher

# 遍历课表文件的每个课程安排小节（如[Monday]等），查找课程名并替换对应老师名字
for day_section in config.sections():
    if day_section!= "Main":
        for cls_num in range(1, 6):
            # 构造课程名对应的配置文件中的键
            course_name_key = f"cls{cls_num}_name"
            # 构造老师名对应的配置文件中的键
            teacher_name_key = f"cls{cls_num}_teacher"
            # 获取课程名
            course_name = config.get(day_section, course_name_key)
            if course_name!= "none":
                if course_name in teacher_dict:
                    # 如果课程名在老师字典中能找到，就将对应的老师名设置到配置文件中相应位置
                    config.set(day_section, teacher_name_key, teacher_dict[course_name])

# 将修改后的内容写回到cfg.ini文件（覆盖原文件内容），你可以根据需要备份原文件
with open('Config/cfg.ini', 'w', encoding='utf-8') as f:
    config.write(f)

# 接下来进行删除指定文件的相关操作

# 定义要删除的文件列表
files_to_delete = ["kebiao.txt", "laoshi.txt", "output.ini"]
for file in files_to_delete:
    try:
        # 使用os.remove方法删除文件，如果文件不存在会抛出FileNotFoundError异常
        os.remove(file)
        print(f"{file} 文件删除成功")
    except FileNotFoundError:
        print(f"{file} 文件不存在，无法删除")
    except OSError as e:
        print(f"删除 {file} 文件时出现其他错误: {e}")