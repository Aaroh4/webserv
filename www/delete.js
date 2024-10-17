function onButtonClick() {
    fetch('images/image.png', {
        method: 'DELETE'
    })
    // .then(response => {
    //     if (!response.ok) {
    //         throw new Error('Request failed');
    //     }
    //     console.log('Image deleted successfully');
    // })
    // .catch(error => console.error('Error:', error));
}

const button = document.querySelector('button');
button.addEventListener('click', onButtonClick);
