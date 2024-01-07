import asyncio
import typing

import requests
from aiogram import Bot, Dispatcher, types

from tea import Tea
from tea_profiles import tp
from config import token

bot = Bot(token=token)
# Объект бота
dp = Dispatcher(bot)
teas = []  # type: typing.List[Tea]

is_choosing = False
is_choosing_sugar_amount = False
is_choosing_tea = False
is_choosing_temperature = False
is_final = False
is_choosing_strength = False
is_choosing_tea_profiles = False
tea_amount = 0
sugar_amount = 0
temperature = 0
strength = 0
# ip_tea = '192.168.1.41'
# ip_tea = '192.168.170.195'

#ip_tea = '192.168.18.79'
ip_tea = '192.169.3.82'


def make_Standart():
    is_choosing = False
    is_choosing_sugar_amount = False
    is_choosing_tea = False
    is_choosing_temperature = False
    is_final = False
    is_choosing_strength = False
    is_choosing_tea_profiles = False
    tea_amount = 0
    sugar_amount = 0
    temperature = 0
    strength = 0


@dp.message_handler(lambda message: message.text == '/start' or message.text == 'Да')
async def cmd_start(message: types.Message):
    global is_choosing, is_choosing_tea, is_choosing_temperature, is_choosing_sugar_amount, is_final
    kb = [
        [
            types.KeyboardButton(text="Заварить чай")],
        [types.KeyboardButton(text="Готовые рецепты")],
        [types.KeyboardButton(text="Что-то еще (Coming soon)")
         ],
    ]
    keyboard = types.ReplyKeyboardMarkup(
        keyboard=kb,
        resize_keyboard=True,
        input_field_placeholder="Выберите что-то"
    )
    is_choosing_sugar_amount = False
    is_choosing_tea = False
    is_choosing_tea_profiles = False
    is_choosing_temperature = False
    is_final = False
    is_choosing = True

    if message.text == '/start':
        await message.answer(f"Добрый день {message.from_user.first_name}, что хотите сделать?", reply_markup=keyboard)
    else:
        await message.answer(f"{message.from_user.first_name}, что хотите сделать?", reply_markup=keyboard)


@dp.message_handler(lambda message: is_choosing)
async def choosing(message: types.Message):
    global is_choosing, is_choosing_tea, is_choosing_tea_profiles
    if message.text == 'Заварить чай':
        is_choosing = False
        await message.answer('Сколько мл воды в чае вы хотите (   не более 300 мл)?',
                             reply_markup=types.ReplyKeyboardRemove())
        is_choosing_tea = True
    elif message.text == 'Готовые рецепты':
        is_choosing = False
        kb = [
            [
                types.KeyboardButton(text="Чёрный чай")],
            [types.KeyboardButton(text="Зелёный чай")],
            [types.KeyboardButton(text="Что-то еще (Coming soon)")
             ],
        ]
        keyboard = types.ReplyKeyboardMarkup(
            keyboard=kb,
            resize_keyboard=True,
            input_field_placeholder="Выберите что-то"
        )
        await message.answer('Выберите рецепт',
                             reply_markup=keyboard)
        is_choosing_tea_profiles = True
    elif message.text == 'Что-то еще (Coming soon)':
        await message.answer('Эта функция пока что не доступна')
        await message.answer(f"{message.from_user.first_name}, что хотите сделать?")
    else:
        await message.answer('Такого варианта нет')
        await message.answer(f"{message.from_user.first_name}, что хотите сделать?")


@dp.message_handler(lambda message: message.text and is_choosing_tea_profiles)
async def profile_tea(message: types.Message):
    global is_choosing_sugar_amount, is_choosing_tea, tea_amount
    if message.text == 'Чёрный чай':
        res = tp['black']
        tea_amount = res[0]
        sugar_amount = res[1]
        temperature = res[2]
        strength = res[3]
        request = f"a={strength}&b={sugar_amount}&c={temperature}&d={tea_amount}"
        await message.answer('Чёрный чай уже готовится',
                             reply_markup=types.ReplyKeyboardRemove())
        print(request)
        print(requests.get(f'http://{ip_tea}/on?{request}'))
        make_Standart()
    elif message.text == 'Зелёный чай':
        res = tp['green']
        tea_amount = res[0]
        sugar_amount = res[1]
        temperature = res[2]
        strength = res[3]
        request = f"a={strength}&b={sugar_amount}&c={temperature}&d={tea_amount}"
        await message.answer('Зелёный чай уже готовится',
                             reply_markup=types.ReplyKeyboardRemove())
        print(request)
        #http://192.168.1.60/on?a=2&b=2&c=40&d=180
        print(requests.get(f'http://{ip_tea}/on?{request}'))
        make_Standart()
    else:
        make_Standart()
        await message.answer('Такого чая нет',
                             reply_markup=types.ReplyKeyboardRemove())
    is_choosing = False
    is_choosing_sugar_amount = False
    is_choosing_tea = False
    is_choosing_temperature = False
    is_final = False
    is_choosing_strength = False
    is_choosing_tea_profiles = False
    tea_amount = 0
    sugar_amount = 0
    temperature = 0


@dp.message_handler(lambda message: message.text and is_choosing_tea)
async def choosing_sugar_amount(message: types.Message):
    global is_choosing_sugar_amount, is_choosing_tea, tea_amount
    if message.text.isnumeric() and 0 <= int(message.text) <= 300:
        tea_amount = int(message.text)
        await message.answer('Сколько чайных ложек сахара вы хотите (не более 10)?')
        is_choosing_sugar_amount = True
        is_choosing_tea = False
    else:
        await message.answer('Столько чая не поместится :(')
        await message.answer('Сколько мл воды в чае вы хотите (не более 300 мл)?')


@dp.message_handler(lambda message: message.text and is_choosing_sugar_amount)
async def choosing_sugar(message: types.Message):
    global is_choosing_sugar_amount, sugar_amount, is_choosing_temperature, is_choosing_strength
    if message.text.isnumeric() and 0 <= int(message.text) <= 10:
        sugar_amount = int(message.text)
        is_choosing_sugar_amount = False
        is_choosing_strength = True
        await message.answer('Какой крепости вы хотите чай (не более 4)?')
    else:
        await message.answer('Столько чайных ложек сахара насыпать не получится :(')
        await message.answer('Сколько чайных ложек сахара вы хотите (не более 10)?')


@dp.message_handler(lambda message: message.text and is_choosing_strength)
async def is_choosing_strength(message: types.Message):
    global is_choosing_temperature, is_choosing_strength, strength
    if message.text.isnumeric() and 1 <= int(message.text) <= 4:
        strength = int(message.text)
        is_choosing_strength = False
        is_choosing_temperature = True
        await message.answer('Какую температуру чая вы хотите (не выше 96 C)?')
    else:
        await message.answer('Такой крепости чая заварить не получится :(')
        await message.answer('Какой крепости вы хотите чай (не более 4)?  ')


@dp.message_handler(lambda message: message.text and is_choosing_temperature)
async def choosing_temp(message: types.Message):
    global is_choosing_temperature, temperature, is_final, sugar_amount, tea_amount
    if message.text.isnumeric() and 0 <= int(message.text) <= 96:
        temperature = int(message.text)
        is_final = True
        is_choosing_temperature = False
        kb = [
            [types.KeyboardButton(text="Заварить чай!")],
            [types.KeyboardButton(text="Отменить")]
        ]
        keyboard = types.ReplyKeyboardMarkup(
            keyboard=kb,
            resize_keyboard=True,
            input_field_placeholder="Выберите что-то"
        )
        await message.answer('Вы выбрали: \n'
                             f'☕️ Объем воды в чае: {tea_amount} мл\n'
                             f'🍰 Количество сахара: {sugar_amount} ч.л.\n'
                             f'☕️ Крепость чая: {strength} порций\n'
                             f'♨️ температура: {temperature} С', reply_markup=keyboard)
    else:
        await message.answer('Такой температуры чай заварить не получится :(')
        await message.answer('Какую температуру чая вы хотите (не выше 96 С)?')


@dp.message_handler(lambda message: message.text and is_final)
async def final(message: types.Message):
    global temperature, is_final, sugar_amount, tea_amount, ip_tea, strength
    if message.text == 'Заварить чай!' or message.text == 'Отменить':
        is_final = False
        if message.text == 'Отменить':
            await message.answer('Вы отменили заварку чая', reply_markup=types.ReplyKeyboardRemove())

        else:
            await message.answer('Ваш чай заваривается! ', reply_markup=types.ReplyKeyboardRemove())
            request = f"a={strength}&b={sugar_amount}&c={temperature}&d={tea_amount}"
            print(requests.get(f'http://{ip_tea}/on?{request}'))
        await cmd_start(message)
    else:
        await message.answer('Такого варианта нету, выберите один из вариантов снизу на клавиатуре')


async def main():
    await dp.start_polling(bot)


if __name__ == "__main__":
    a = asyncio.get_event_loop()
    a.run_until_complete(main())
