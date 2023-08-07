import asyncio
import typing

import requests
from aiogram import Bot, Dispatcher, types

from tea import Tea

# Объект бота
bot = Bot(token="6251711417:AAGPwxSdXCMHfyhzISqrqXMhwpJ326TdRqQ")
# Диспетчер
dp = Dispatcher(bot)
teas = [] # type: typing.List[Tea]

is_choosing = False
is_choosing_sugar_amount = False
is_more = False
is_edit = False
is_ready = False
is_question = False
is_delete = False
is_choosing_strength = False
is_strength_edit = False
is_sugar_edit = False
cur_tea_amount = 0
tea_amount = 0
sugar_amount = 0
cur_sugar_amount = 0
ip_tea = '192.168.1.41'


@dp.message_handler(lambda message: message.text == '/start' or message.text == 'Заварить еще чая')
async def cmd_start(message: types.Message):
    global is_choosing
    kb = [
        [
            types.KeyboardButton(text="Заварить чай"),
            types.KeyboardButton(text="Что-то еще (Coming soon)")
        ],
    ]
    keyboard = types.ReplyKeyboardMarkup(
        keyboard=kb,
        resize_keyboard=True,
        input_field_placeholder="Выберите что-то"
    )
    is_choosing = True
    await message.answer(f"Добрый день {message.from_user.first_name}, что хотите сделать?", reply_markup=keyboard)


@dp.message_handler(lambda message: is_choosing)
async def choosing(message: types.Message):
    global is_choosing, is_choosing_strength, cur_sugar_amount
    if message.text == 'Заварить чай':
        is_choosing = False
        await message.answer('Насколько крепкий вы хотите чай? (от 1 до 4)',
                             reply_markup=types.ReplyKeyboardRemove())
        is_choosing_strength = True

    elif message.text == 'Что-то еще (Coming soon)':
        await message.answer('Эта функция пока что не доступна')
        await message.answer(f"{message.from_user.first_name}, что хотите сделать?")
    else:
        await message.answer('Такого варианта нет')
        await message.answer(f"{message.from_user.first_name}, что хотите сделать?")


@dp.message_handler(lambda message: message.text.isnumeric() and is_choosing_sugar_amount)
async def choosing_sugar_amount(message: types.Message):
    global is_choosing_sugar_amount, is_choosing_strength, cur_sugar_amount
    if 0 <= int(message.text) <= 4:
        cur_sugar_amount = int(message.text)
        await message.answer('Насколько крепкий вы хотите чай? (от 1 до 4)')
        is_choosing_sugar_amount = False
        is_choosing_strength = True
    else:
        await message.answer('Столько чая не поместится :(')
        await message.answer('Сколько чайных ложек сахара вы хотите? (от 0 до 4)')


@dp.message_handler(lambda message: message.text.isnumeric() and is_choosing_strength)
async def ready(message: types.Message):
    global is_ready, teas, is_choosing_strength, is_ready
    if 1 <= int(message.text) <= 4:
        teas.append(Tea(cur_sugar_amount, int(message.text)))
        is_choosing_strength = False
        is_ready = True
        kb = [
            [types.KeyboardButton(text="Всё хорошо")],
            [types.KeyboardButton(text="Хочу изменить")]
        ]
        keyboard = types.ReplyKeyboardMarkup(
            keyboard=kb,
            resize_keyboard=True,
            input_field_placeholder="Выберите что-то"
        )
        await message.answer(
            f'Вы выбрали: \n{message.text} - крепость чая\nХотите изменить?',
            reply_markup=keyboard)
    else:
        await message.answer('Чай такой крепости заварить не получится :(')
        await message.answer('Насколько крепкий вы хотите чай? (от 1 до 4)')


@dp.message_handler(lambda message: is_ready)
async def do_edit(message: types.Message):
    global is_ready, teas, is_choosing_strength, is_ready, is_edit
    if message.text == 'Хочу изменить':
        is_ready = False
        is_edit = True
        kb = [
            [
                types.KeyboardButton(text="Крепость чая")
            ],
        ]
        keyboard = types.ReplyKeyboardMarkup(
            keyboard=kb,
            resize_keyboard=True,
            input_field_placeholder="Выберите что-то"
        )
        await message.answer('Что вы хотите изменить?', reply_markup=keyboard)

    elif message.text == 'Всё хорошо':
        kb = [
            [
                types.KeyboardButton(text="Заварить еще чая"),
            ],
        ]
        keyboard = types.ReplyKeyboardMarkup(
            keyboard=kb,
            resize_keyboard=True,
            input_field_placeholder="Выберите что-то"
        )

        await message.answer('Хорошо, ваш чай готовится :)', reply_markup=keyboard)
        print(1)
        request = f"{teas[0].strength},0,80,200;"
        print(requests.post(f'http://{ip_tea}/tea', params={'tea':request}))
        # requests.post('http://192.168.5.5/on', params={})
        is_ready = False
        teas = []


@dp.message_handler(lambda message: is_edit)
async def edit(message: types.Message):
    global is_ready, teas, is_choosing_strength, is_ready, is_edit, is_strength_edit, is_sugar_edit
    if message.text == 'Крепость чая':
        await message.answer('Какую крепость чая вы хотите (от 1 до 4)?', reply_markup=types.ReplyKeyboardRemove())
        is_strength_edit = True
        is_edit = False
    elif message.text == 'Количество сахара':
        await message.answer('Сколько чайных ложек сахара вы хотите (от 0 до 4)?',
                             reply_markup=types.ReplyKeyboardRemove())
        is_sugar_edit = True
        is_edit = False


@dp.message_handler(lambda message: is_strength_edit)
async def edit_strength(message: types.Message):
    global is_strength_edit, is_ready
    if message.text.isnumeric() and (1 <= int(message.text) <= 4):
        teas[0].strength = int(message.text)
        is_strength_edit = False
        is_ready = True
        kb = [
            [
                types.KeyboardButton(text="Всё хорошо"),
                types.KeyboardButton(text="Хочу изменить")
            ],
        ]
        keyboard = types.ReplyKeyboardMarkup(
            keyboard=kb,
            row_width=1,
            resize_keyboard=True,
            input_field_placeholder="Выберите что-то"
        )
        await message.answer(
            f'Вы выбрали: \n{teas[0].strength} - Крепость чая',
            reply_markup=keyboard)
    else:
        await message.answer('Чай такой крепости заварить не получится :(')
        await message.answer('Насколько крепкий вы хотите чай? (от 1 до 4)')


# @dp.message(lambda message: is_sugar_edit)
# async def edit_sugar(message: types.Message):
#     global is_sugar_edit, is_ready
#     if message.text.isnumeric() and (0 <= int(message.text) <= 4):
#         teas[0].sugar = int(message.text)
#         is_sugar_edit = False
#         is_ready = True
#         kb = [
#             [
#                 types.KeyboardButton(text="Да, хочу изменить"),
#                 types.KeyboardButton(text="Нет")
#             ],
#         ]
#         keyboard = types.ReplyKeyboardMarkup(
#             keyboard=kb,
#             resize_keyboard=True,
#             input_field_placeholder="Выберите что-то"
#         )
#         await message.answer(
#             f'Вы выбрали: \n{teas[0].sugar} - Количество чайных ложек сахара\n{teas[0].strength} - Крепость чая',
#             reply_markup=keyboard)
#     else:
#         await message.answer('Столько чайных ложек сахара положить не получится :(')
#         await message.answer('Сколько чайных ложек сахара вы хотите? (от 0 до 4)')


async def main():
    await dp.start_polling(bot)


if __name__ == "__main__":
    a = asyncio.get_event_loop()
    a.run_until_complete(main())
