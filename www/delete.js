function onButtonClick() {
	fetch('/images/image.png', {
		method: 'DELETE'
	})
}

const button = document.querySelector('button');
button.addEventListener('click', onButtonClick);