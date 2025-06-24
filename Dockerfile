# Use the official lightweight Nginx image.
FROM nginx:stable-alpine

# Maintainer information (update as needed)
LABEL maintainer="Your Name <woowarey@gmail.com>"

# Remove the default Nginx configuration.
RUN rm /etc/nginx/conf.d/default.conf

# Copy a custom Nginx configuration file if desired (optional).
# COPY nginx.conf /etc/nginx/conf.d/

# Copy the build output from the 'dist/' directory into Nginx's serving directory.
COPY dist/ /usr/share/nginx/html

# Expose port 80 to be accessible from the host.
EXPOSE 80

# Run Nginx in the foreground.
CMD ["nginx", "-g", "daemon off;"]
