from django.shortcuts import render , get_object_or_404
from polls.form import ImageForm
from polls.models import ImageFile, Resume
from django.http import HttpResponseRedirect
from django.urls import reverse

def index(request):
    #데이터받기
    if request.method == 'POST':
        resume = Resume()
        resume.name = request.POST['name']
        resume.number = request.POST['number']
        resume.birthday = request.POST['birthday']

        
        resume.email = request.POST['email']
        resume.address = request.POST['address']
        #학교명, 학위, 기간, 전공, 졸업여부
        resume.education1_name = request.POST['education1_name']
        resume.education2_name = request.POST['education2_name']
        resume.education3_name = request.POST['education3_name']

        resume.education1_degreeCourse = request.POST['education1_degreeCourse']
        resume.education2_degreeCourse = request.POST['education2_degreeCourse']
        resume.education3_degreeCourse = request.POST['education3_degreeCourse']

        resume.education1_term = request.POST['education1_term']
        resume.education2_term = request.POST['education2_term']
        resume.education3_term = request.POST['education3_term']

        resume.education1_major = request.POST['education1_major']
        resume.education2_major = request.POST['education2_major']
        resume.education3_major = request.POST['education3_major']

        resume.education1_graduate = request.POST['education1_graduate']
        resume.education2_graduate = request.POST['education2_graduate']
        resume.education3_graduate = request.POST['education3_graduate']
        #특이사항
        resume.note = request.POST['note']
        #이미지
        if 'image' in request.FILES:
            resume.images = request.FILES['image']
        #저장
        resume.save()
        #def imageispla실행
        return HttpResponseRedirect(reverse('imagedisplay', args=(resume.id,)))


    else:
        #기본접속
        image_form = ImageForm()
        return render(request,'polls/index.html', {'image_form':image_form})

def imagedisplay(request, imgid):
    #데이터 확인후 넘김
    img=get_object_or_404(Resume, pk=imgid)
    return render(request, 'polls/imagedisplay.html', {'img':img})