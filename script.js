// Fixed CTA Bar - Show on scroll
window.addEventListener('scroll', function() {
    const fixedCta = document.getElementById('fixedCta');
    const heroSection = document.querySelector('.hero');

    if (heroSection) {
        const heroBottom = heroSection.offsetTop + heroSection.offsetHeight;

        if (window.scrollY > heroBottom) {
            fixedCta.classList.add('visible');
        } else {
            fixedCta.classList.remove('visible');
        }
    }
});

// Smooth scrolling for anchor links
document.querySelectorAll('a[href^="#"]').forEach(anchor => {
    anchor.addEventListener('click', function (e) {
        e.preventDefault();
        const target = document.querySelector(this.getAttribute('href'));

        if (target) {
            const offset = 80; // Account for fixed header if needed
            const targetPosition = target.offsetTop - offset;

            window.scrollTo({
                top: targetPosition,
                behavior: 'smooth'
            });
        }
    });
});

// Form submission handler
const bookingForm = document.getElementById('bookingForm');
if (bookingForm) {
    bookingForm.addEventListener('submit', function(e) {
        e.preventDefault();

        // Get form data
        const formData = new FormData(bookingForm);

        // Here you would typically send the data to your backend or booking service
        // For now, we'll show an alert
        alert('Thank you for your interest! We will contact you shortly to schedule your consultation.');

        // Reset form
        bookingForm.reset();

        // In a real implementation, you might integrate with:
        // - Calendly API
        // - Google Calendar API
        // - Your own booking system
        // - Email service (SendGrid, Mailgun, etc.)

        // Example integration with a backend:
        /*
        fetch('/api/book-consultation', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(Object.fromEntries(formData))
        })
        .then(response => response.json())
        .then(data => {
            if (data.success) {
                // Redirect to thank you page or show confirmation
                window.location.href = '/thank-you';
            }
        })
        .catch(error => {
            console.error('Error:', error);
            alert('Something went wrong. Please try again or email us directly.');
        });
        */
    });
}

// Intersection Observer for fade-in animations (optional enhancement)
const observerOptions = {
    threshold: 0.1,
    rootMargin: '0px 0px -50px 0px'
};

const observer = new IntersectionObserver(function(entries) {
    entries.forEach(entry => {
        if (entry.isIntersecting) {
            entry.target.style.opacity = '1';
            entry.target.style.transform = 'translateY(0)';
        }
    });
}, observerOptions);

// Apply fade-in to cards and sections
document.addEventListener('DOMContentLoaded', function() {
    const animateElements = document.querySelectorAll(
        '.problem-card, .solution-card, .case-study-card, .process-step, .faq-item'
    );

    animateElements.forEach(el => {
        el.style.opacity = '0';
        el.style.transform = 'translateY(20px)';
        el.style.transition = 'opacity 0.6s ease, transform 0.6s ease';
        observer.observe(el);
    });
});

// Track CTA clicks for analytics (optional)
document.querySelectorAll('.btn').forEach(button => {
    button.addEventListener('click', function() {
        const buttonText = this.textContent.trim();

        // Send to analytics
        if (typeof gtag !== 'undefined') {
            gtag('event', 'cta_click', {
                'event_category': 'engagement',
                'event_label': buttonText
            });
        }

        // Or use your preferred analytics tool
        console.log('CTA clicked:', buttonText);
    });
});
