function onButtonClick() {
	fetch('/video.mp4', {
		method: 'DELETE'
	})
}

const button = document.querySelector('button');
button.addEventListener('click', onButtonClick);