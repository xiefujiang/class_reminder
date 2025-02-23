import requests
import base64
from PIL import Image
from io import BytesIO
import ddddocr
import configparser
import re

# 读取配置文件
config = configparser.ConfigParser()
config.read('Config/cfg.ini', encoding='utf-8')

# 从配置文件中获取变量（除了urls相关，其他配置项仍从文件读取）
userAccount = config.get('Main', 'account')
userPassword = config.get('Main', 'password')
rq = config.get('Main', 'checking_date')

# 写死的urls地址，根据实际情况替换成真实有效的地址
captcha_url = "https://jwxt.sut.edu.cn/jsxsd/verifycode.servlet"
login_url = "https://jwxt.sut.edu.cn/jsxsd/xk/LoginToXk"
target_url_with_load = "https://jwxt.sut.edu.cn/jsxsd/framework/main_index_loadkb.jsp"
target_url_without_load = "https://jwxt.sut.edu.cn/jsxsd/xskb/xskb_list.do"
sjmsValue = "9568AEA6C1082FBDE0530100007F90E3"


def execute_operations():
    """
    封装执行一次完整操作的函数，包括获取验证码、登录、访问目标页面等操作
    """
    # Base64编码userAccount和userPassword，并使用'%%%'作为分隔符
    encoded_userAccount = base64.b64encode(userAccount.encode('utf-8')).decode('utf-8')
    encoded_userPassword = base64.b64encode(userPassword.encode('utf-8')).decode('utf-8')
    encoded_string = encoded_userAccount + '%%%' + encoded_userPassword

    # 创建一个Session对象来管理cookies和会话
    session = requests.Session()

    # 获取验证码图片
    response = session.get(captcha_url)
    if response.status_code == 200:
        # 使用PIL和ddddocr库读取并识别验证码
        captcha_image = Image.open(BytesIO(response.content))
        ocr = ddddocr.DdddOcr()
        captcha_text = ocr.classification(captcha_image)
        print(f'识别出的验证码是: {captcha_text}')

        # 构造登录表单数据
        login_data = {
            'userAccount': userAccount,
            'userPassword': '',  # 根据您的要求，这里为空
            'RANDOMCODE': captcha_text,  # 填入识别出的验证码
            'encoded': encoded_string  # 使用Base64编码的userAccount和userPassword组合
        }

        # 发送登录请求
        login_response = session.post(login_url, data=login_data)

        # 检查登录是否成功
        if login_response.status_code == 200:  # 假设登录成功状态码为200
            print('登录请求已发送，请检查响应以确定是否成功登录。')

            # 如果登录成功，构造要发送到目标页面的负载数据
            target_post_data = {
                'rq': rq,
                'sjmsValue': sjmsValue,
            }

            # 访问第一个目标页面并POST负载数据
            target_response_with_load = session.post(target_url_with_load, data=target_post_data)

            if target_response_with_load.status_code == 200:
                # 打印目标页面的响应
                print(target_response_with_load.text)

                # 保存响应数据到本地文件
                with open('kebiao.txt', 'w', encoding='utf-8') as f:
                    f.write(target_response_with_load.text)
                    print('第一个目标页面（带负载）的响应数据已保存到本地文件。')
            else:
                print(f'无法访问第一个目标页面或POST请求失败，状态码: {target_response_with_load.status_code}')

            # 访问第二个目标页面（不带负载）
            target_response_without_load = session.get(target_url_without_load)

            if target_response_without_load.status_code == 200:
                # 打印目标页面的响应
                print(target_response_without_load.text)

                # 保存响应数据到本地文件
                with open('laoshi.txt', 'w', encoding='utf-8') as f:
                    f.write(target_response_without_load.text)
                    print('第二个目标页面（不带负载）的响应数据已保存到本地文件。')
            else:
                print(f'无法访问第二个目标页面，状态码: {target_response_without_load.status_code}')

        else:
            print(f'登录失败，状态码: {login_response.status_code}')
    else:
        print(f'无法获取验证码图片，状态码: {response.status_code}')


# 循环三次执行操作
for _ in range(3):
    execute_operations()

# 以下是对保存到本地文件内容进行清理和过滤的函数及相关逻辑

def clean_html_except_paragraphs_and_format(file_path):
    """
    函数功能：对给定文件路径的文件内容进行一系列清理操作，主要清理HTML相关的多余属性、标签等，只保留段落文本，并进行一些特定文本的替换和空行处理等。

    参数说明：
    file_path：需要清理内容的文件的路径

    具体清理步骤如下：
    1. 读取文件内容。
    2. 依次使用正则表达式移除style属性、<p>标签中的title属性、“课程属性：必修”等特定文本、“分组名：...”相关文本。
    3. 使用正则表达式匹配所有<p>...</p>块，提取其中内容，将<br/>替换为换行符构建新内容，移除一些括号相关文本以及多余空行。
    4. 新增功能：删除包含“'”的行中该字符及其之后的所有内容。
    5. 将清理后的内容写回原文件。
    """
    # 读取文件内容
    with open(file_path, 'r', encoding='utf-8') as file:
        content = file.read()

        # 移除 style 属性
        content_without_style = re.sub(r'style=\'text-align: left;font-size: 12px;font-weight: bold;\'', '', content)

        # 移除 <p> 标签中的 title 属性
        content_without_title = re.sub(r'<p\s+title="[^"]*"', '<p', content_without_style)

        # 移除“课程属性：必修”
        content_without_course_attribute = re.sub(r'<p>课程属性：必修<br/>', '', content_without_title)

        # 移除类似“分组名：网球一班'  >大学体育（一..”的文本
        # 注意：这个正则表达式可能需要根据你的具体文本内容进行调整
        content_without_group_name = re.sub(r'<p>分组名：[^<]*>[^<]*（一\.\.', '', content_without_course_attribute)

        # 使用正则表达式匹配所有<p>...</p>块，不考虑段落间的格式
        paragraphs = re.findall(r'<p[^>]*>(.*?)</p>', content_without_group_name, re.DOTALL)

        # 在每个段落内部，将<br/>替换为换行符
        cleaned_paragraphs = [re.sub(r'<br/>', '\n', paragraph) for paragraph in paragraphs]

        # 构建新的内容，只包含段落文本，不包含<p>和</p>标签
        cleaned_content_with_empty_lines = '\n'.join(cleaned_paragraphs)

        # 移除“（一..”和“（..”
        cleaned_content_without_parentheses = re.sub(r'（一\.\.', '', cleaned_content_with_empty_lines)
        cleaned_content_without_parentheses = re.sub(r'（\.\.', '', cleaned_content_without_parentheses)
        cleaned_content_without_parentheses = re.sub(r'\.\.', '', cleaned_content_without_parentheses)
        cleaned_content_without_parentheses = re.sub(r'课程属性：必修', '', cleaned_content_without_parentheses)
        cleaned_content_without_parentheses = re.sub(r'分组名：网球一班\'  \>大学体育', '',
                                                     cleaned_content_without_parentheses)

        # 移除所有空行
        cleaned_content = re.sub(r'\n+', '\n', cleaned_content_without_parentheses.strip())

        # 新增功能：删除包含“'”的行中该字符及其之后的所有内容
        cleaned_lines = []
        for line in cleaned_content.split('\n'):
            if "'" in line:
                line = line.split("'")[0]  # 只保留"'"之前的部分
            cleaned_lines.append(line)

            # 重新组合清理后的行
        final_cleaned_content = '\n'.join(cleaned_lines)

        # 将清理后的内容写回文件
    with open(file_path, 'w', encoding='utf-8') as file:
        file.write(final_cleaned_content)
    print(f"清理后的内容已写入 {file_path}")


def filter_and_clean_lines(file_path, keyword, remove_prefix, remove_suffix, replace_string, replacement,
                           replace_parentheses):
    """
    函数功能：对给定文件路径的文件内容进行过滤和清理操作，基于关键字保留特定行，并对这些行进行前缀、后缀删除，字符串替换，以及其他一些特定的文本清理和去重操作。

    参数说明：
    file_path：需要处理内容的文件的路径
    keyword：用于筛选行的关键字，只保留包含该关键字的行
    remove_prefix：需要删除的每行的前缀内容，及其之前的所有内容都会被删除
    remove_suffix：需要删除的每行的后缀内容，及其之后的所有内容都会被删除
    replace_string：需要被替换的字符串
    replacement：用来替换replace_string的内容
    replace_parentheses：是否进行括号替换相关操作的布尔值

    具体处理步骤如下：
    1. 读取文件内容，使用列表推导式基于关键字筛选出特定行。
    2. 对筛选出的行进行处理，根据是否找到前缀和后缀，删除相应内容并进行字符串替换。
    3. 删除每行的"</font>"、"<br/>"、">"符号等特定内容。
    4. 如果replace_parentheses为True，对包含“大学体育（一）”的行进行括号替换操作。
    5. 对处理后的行进行去重处理，然后将处理后的行准备好写入文件的格式（可选择添加空行），最后写回原文件。
    """
    # 使用with语句确保文件正确关闭
    with open(file_path, 'r', encoding='utf-8') as infile:
        lines = infile.readlines()

        # 使用列表推导式只保留包含关键字的行
    filtered_lines = [line for line in lines if keyword in line]

    # 对保留的行进行处理，删除指定前缀及其之前的所有内容，以及指定后缀及其之后的所有内容，并进行字符串替换
    cleaned_lines = []
    for line in filtered_lines:
        index_prefix = line.find(remove_prefix)
        index_suffix = line.find(remove_suffix)

        if index_prefix!= -1 and index_suffix!= -1:
            # 如果找到了指定前缀和后缀，则只保留前缀之后、后缀之前的内容，并进行替换
            cleaned_line = line[index_prefix + len(remove_prefix):index_suffix].replace(replace_string, replacement)
        elif index_prefix!= -1:
            # 如果只找到了前缀，则删除前缀及其之前的内容，并进行替换
            cleaned_line = line[index_prefix + len(remove_prefix):].replace(replace_string, replacement)
        elif index_suffix!= -1:
            # 如果只找到了后缀（这种情况不太可能，因为我们是基于关键字过滤的），则保留整行直到后缀之前的内容，并进行替换
            # 但为了代码的完整性，这里还是进行了处理
            cleaned_line = line[:index_suffix].replace(replace_string, replacement)
        else:
            # 如果没有找到前缀或后缀（这种情况也不应该发生），则不处理该行
            continue

            # 删除每行的 "</font>" 和 "<br/>"
        cleaned_line = cleaned_line.replace("</font>", "").replace("<br/>", "").strip()

        # 删除每行的 ">" 符号（新增功能）
        cleaned_line = cleaned_line.replace(">", "").strip()

        # 将“大学体育（一）”之后的()替换为（）
        # 注意：这里假设“大学体育（一）”之后只会有一个()需要替换，如果有多个，可能需要使用正则表达式
        index_sport = cleaned_line.find("大学体育（一）")
        if index_sport!= -1:
            # 找到了“大学体育（一）”，进行替换
            # 注意这里只替换了第一个出现的()，如果需要替换所有，请使用正则表达式
            open_paren_index = cleaned_line.find("(", index_sport)
            close_paren_index = cleaned_line.find(")", open_paren_index)
            if open_paren_index!= -1 and close_paren_index!= -1:
                cleaned_line = (cleaned_line[:open_paren_index] +
                                "（" + cleaned_line[open_paren_index + 1:close_paren_index] + "）" +
                                cleaned_line[close_paren_index + 1:])

                # 将处理后的行添加到结果列表中
        cleaned_lines.append(cleaned_line)

        # 去重处理
    unique_lines = list(set(cleaned_lines))

    # 准备写入文件的内容，每行之间添加一个空行（根据需求，也可以不添加）
    output_lines = []
    for line in unique_lines:
        output_lines.append(line)
        output_lines.append('\n')  # 如果不需要每行之间的空行，可以注释或删除这一行

    # 如果不需要最后的额外空行，可以移除它（根据需求决定）
    if output_lines and output_lines[-1] == '\n':
        output_lines.pop()

        # 写入文件，注意这将覆盖原始文件（也可以选择写入新文件）
    with open(file_path, 'w', encoding='utf-8') as outfile:
        outfile.writelines(output_lines)


if __name__ == "__main__":
    kebiao_file_path = 'kebiao.txt'  # 对应第一个功能的输入（也是输出）文件路径
    clean_html_except_paragraphs_and_format(kebiao_file_path)

    laoshi_file_path = 'laoshi.txt'  # 对应第二个功能的输入和输出文件路径
    keyword = "老师"
    remove_prefix = "class=\"kbcontent\""
    remove_suffix = "<br/><font title='周次(节次)'>"
    replace_string = "<br/><font title='老师'>"
    replacement = " "
    replace_parentheses = True
    filter_and_clean_lines(laoshi_file_path, keyword, remove_prefix, remove_suffix, replace_string, replacement,
                           replace_parentheses)
    print(
        f"处理了文件 {laoshi_file_path}，仅保留了包含关键字 '{keyword}' 的行，并删除了每行中 '{remove_prefix}' 及其之前的所有内容和 '{remove_suffix}' 及其之后的所有内容，"
        f"同时将每行的 '{replace_string}' 替换为 '{replacement}'，删除了每行的 '</font>' 和 '<br/>' 以及 '>'，并将“大学体育（一）”之后的()替换为（），并进行了文本去重。")