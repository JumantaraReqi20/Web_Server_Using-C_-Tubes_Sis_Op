document.addEventListener('DOMContentLoaded', () => {
    // Navigation section switching
    const navLinks = document.querySelectorAll('nav a');
    const sections = document.querySelectorAll('main section');

    navLinks.forEach(link => {
        link.addEventListener('click', (e) => {
            e.preventDefault();
            const targetSection = link.getAttribute('data-section');

            // Hide all sections
            sections.forEach(section => {
                section.classList.remove('active'); if (section.id === targetSection) {
                    section.classList.add('active'); // Show the target section
                }
            });
        });
    });

    // Contact form submission
    const contactForm = document.getElementById('contact-form');
    contactForm.addEventListener('submit', (e) => {
        e.preventDefault();
        alert('Thank you for your message! We will get back to you soon.');
        contactForm.reset(); // Reset the form after submission
    });
});