from django import forms
class ImageForm(forms.Form):
    img = forms.ImageField(label='')