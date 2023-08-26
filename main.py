from kivy.app import App
from kivy.lang import Builder
from kivy.uix.boxlayout import BoxLayout
from kivy.properties import StringProperty
import requests
from kivy.uix.button import Button
from kivy.uix.textinput import TextInput
from kivy.clock import mainthread
from kivy.core.clipboard import Clipboard as Cb
import threading
import socket

KV = """
MyBL:
	orientation: "vertical"
	size_hint: (0.95, 0.95)
	pos_hint: {"center_x": 0.5, "center_y":0.5}

	Label:
		font_size: "15sp"
		multiline: True
		text_size: self.width*0.98, None
		size_hint_x: 1.0
		size_hint_y: None
		height: self.texture_size[1] + 15
		text: root.data_label
		markup: True
		on_ref_press: root.linki()		


		
	BoxLayout:
        Label:
            font_size: "15sp"
            multiline: True
            text_size: self.width*0.98, None
            size_hint_x: 1.0
            size_hint_y: None
            height: self.texture_size[1] + 15
            width: self.texture_size[0] + 15
            text: "Крепость чая"
            markup: True
        TextInput:
            id: tea
            multiline: False
            padding_y: (5,5)
            size_hint: (1, 0.5)
            on_text: app.process()
            
    BoxLayout:
        Label:
            font_size: "15sp"
            multiline: True
            text_size: self.width*0.98, None
            size_hint_x: 1.0
            size_hint_y: None
            height: self.texture_size[1] + 15
            width: self.texture_size[0] + 15
            text: "Кол-во сахара"
            markup: True
        TextInput:
            id: sugar
            multiline: False
            padding_y: (5,5)
            size_hint: (1, 0.5)
            on_text: app.process()
            
    BoxLayout:
        Label:
            font_size: "15sp"
            multiline: True
            text_size: self.width*0.98, None
            size_hint_x: 1.0
            size_hint_y: None
            height: self.texture_size[1] + 15
            width: self.texture_size[0] + 15
            text: "Температура"
            markup: True
        TextInput:
            id: temp
            multiline: False
            padding_y: (5,5)
            size_hint: (1, 0.5)
            on_text: app.process()
            
    BoxLayout:
        Label:
            font_size: "15sp"
            multiline: True
            text_size: self.width*0.98, None
            size_hint_x: 1.0
            size_hint_y: None
            height: self.texture_size[1] + 15
            width: self.texture_size[0] + 15
            text: "Объём"
            markup: True
        TextInput:
            id: v
            multiline: False
            padding_y: (5,5)
            size_hint: (1, 0.5)
            on_text: app.process()
    
    
    BoxLayout:
        Label:
            font_size: "15sp"
            multiline: True
            text_size: self.width*0.98, None
            size_hint_x: 1.0
            size_hint_y: None
            height: self.texture_size[1] + 15
            width: self.texture_size[0] + 15
            text: "IP адрес"
            markup: True
        TextInput:
            id: ip_adr
            text: "192.168.1.41"
            multiline: False
            padding_y: (5,5)
            size_hint: (1, 0.5)
            on_text: app.process()
    
    Button:
		text: "Отправить"
		bold: True
		padding: 10
        spacing: 10
		padding_y: (5)
		background_color:'#00FFCE'
		size_hint: (1,0.5)
		on_press: root.callback()

"""

n = 0


class MyBL(BoxLayout):
    data_label = StringProperty("Чаеварка!")

    def callback(self):
        global n
        n += 1
        self.data_label = f"Отправленно {n}"
        ip_tea = self.ids.ip_adr.text
        strength = self.ids.tea.text
        sugar_amount = self.ids.sugar.text
        temperature = self.ids.temp.text
        tea_amount = self.ids.v.text

        request = f"a={strength}&b={sugar_amount}&c={temperature}&d={tea_amount}"
        print(request)
        try:
            requests.get(f'http://{ip_tea}/on?{request}')
            self.data_label = f"Чаёчек)))"
        except Exception as e:
            self.data_label = f"Запрос не дошёл, ошибочка(((: {e}"


class MyApp(App):
    running = True

    def process(self):
        text = self.root.ids.ip_adr.text

    def build(self):
        return Builder.load_string(KV)

    def on_stop(self):
        self.running = False


MyApp().run()