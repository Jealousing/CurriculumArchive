from django.db import models
class ImageFile(models.Model):
    photo = models.ImageField(upload_to='img')

#받아올 정보
class Resume(models.Model):
    name = models.TextField(null = True)
    number = models.TextField(null = True)
    birthday = models.TextField(null = True)

    email = models.TextField(null = True)
    address = models.TextField(null = True)

    education1_name = models.TextField(null = True)
    education1_degreeCourse = models.TextField(null=True)
    education1_term = models.TextField(null=True)
    education1_major = models.TextField(null=True)
    education1_graduate = models.TextField(null=True)

    education2_name = models.TextField(null=True)
    education2_degreeCourse = models.TextField(null=True)
    education2_term = models.TextField(null=True)
    education2_major = models.TextField(null=True)
    education2_graduate = models.TextField(null=True)

    education3_name = models.TextField(null=True)
    education3_degreeCourse = models.TextField(null=True)
    education3_term = models.TextField(null=True)
    education3_major = models.TextField(null=True)
    education3_graduate = models.TextField(null=True)

    note = models.TextField(null = True)

    images = models.ImageField(blank=True, upload_to="img", null=True)
    def __str__(self):
        return self.name