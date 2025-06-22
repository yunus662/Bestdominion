// notification.js
// Production‑Quality Notification System for Conqueror Engine
//
// This module provides a robust system for displaying in‑game notifications
// (such as success messages, warnings, and errors). It creates a floating container
// on the page (if one does not yet exist) and dynamically injects notifications which
// fade in, remain visible for a configurable duration, and then fade out.
//
// Usage:
//   // Import or ensure this file is loaded in your HTML.
//   showNotification("Mission accomplished!", { type: "success", duration: 4000 });
//   showNotification("Insufficient funds for purchase.", { type: "warning", importance: 2 });
//
// Notification types supported: "success", "info", "warning", "error".
// Importance can influence styling (for example, larger font or different icon).
// You may adjust the default styling via CSS; this module also injects necessary default CSS if not present.

(function () {
  'use strict';

  // --------------------------------------------------
  // Global Default Settings
  // --------------------------------------------------
  const DEFAULT_DURATION = 5000; // in milliseconds
  const DEFAULT_CONTAINER_ID = 'notification-container';

  // --------------------------------------------------
  // Utility: Create default styles if not present
  // --------------------------------------------------
  function injectDefaultStyles() {
    if (document.getElementById('notification-styles')) return;

    const style = document.createElement('style');
    style.id = 'notification-styles';
    style.type = 'text/css';
    style.innerHTML = `
      /* Notification container */
      #${DEFAULT_CONTAINER_ID} {
        position: fixed;
        top: 20px;
        right: 20px;
        z-index: 10000;
        display: flex;
        flex-direction: column;
        align-items: flex-end;
      }
      /* Notification base style */
      .notification {
        min-width: 250px;
        margin-bottom: 10px;
        padding: 15px 20px;
        border-radius: 4px;
        color: #fff;
        font-family: Arial, sans-serif;
        box-shadow: 0 2px 4px rgba(0,0,0,0.2);
        opacity: 0;
        transform: translateX(100%);
        animation: slideIn 0.5s forwards;
      }
      /* Notification types */
      .notification.success { background-color: #28a745; }
      .notification.info { background-color: #17a2b8; }
      .notification.warning { background-color: #ffc107; color: #333; }
      .notification.error { background-color: #dc3545; }
      /* Importance can adjust border width */
      .notification.importance-1 { border-left: 4px solid #fff; }
      .notification.importance-2 { border-left: 6px solid #fff; }
      .notification.importance-3 { border-left: 8px solid #fff; }
      /* Fade out animation */
      @keyframes fadeOut {
        from { opacity: 1; }
        to { opacity: 0; transform: translateX(100%); }
      }
      /* Slide in animation */
      @keyframes slideIn {
        from { opacity: 0; transform: translateX(100%); }
        to { opacity: 1; transform: translateX(0); }
      }
    `;
    document.head.appendChild(style);
  }

  // --------------------------------------------------
  // Utility: Get or create notification container
  // --------------------------------------------------
  function getNotificationContainer() {
    let container = document.getElementById(DEFAULT_CONTAINER_ID);
    if (!container) {
      container = document.createElement('div');
      container.id = DEFAULT_CONTAINER_ID;
      document.body.appendChild(container);
    }
    return container;
  }

  // --------------------------------------------------
  // Display a notification with given message and options.
  // Options object can include:
  //    type: "success", "info", "warning", "error" (default: "info")
  //    duration: Duration in milliseconds (default: DEFAULT_DURATION)
  //    importance: Numeric level (1 to 3) to adjust visual prominence (default: 1)
  // --------------------------------------------------
  function showNotification(message, options = {}) {
    try {
      const type = options.type || "info";
      const duration = options.duration || DEFAULT_DURATION;
      const importance = options.importance || 1;

      // Create the notification element.
      const notification = document.createElement('div');
      notification.className = `notification ${type} importance-${importance}`;
      notification.textContent = message;

      // Append to container.
      const container = getNotificationContainer();
      container.appendChild(notification);

      // After display duration, fade out and remove.
      setTimeout(() => {
        notification.style.animation = `fadeOut 0.5s forwards`;
        // Remove element after animation ends.
        notification.addEventListener('animationend', () => {
          if (notification.parentNode) {
            notification.parentNode.removeChild(notification);
          }
        });
      }, duration);
    } catch (error) {
      console.error("[ERROR] Failed to show notification:", error);
    }
  }

  // --------------------------------------------------
  // Expose the function globally.
  // --------------------------------------------------
  window.showNotification = showNotification;

  // Optionally, you can add a logging wrapper:
  function logAndNotify(message, options = {}) {
    // Log to console for debugging.
    console.log(`[NOTIFICATION] ${message}`);
    // Show notification.
    showNotification(message, options);
  }
  window.logAndNotify = logAndNotify;

  // Inject default CSS upon module load.
  injectDefaultStyles();

  // For debugging: expose a function to clear all notifications
  function clearAllNotifications() {
    const container = document.getElementById(DEFAULT_CONTAINER_ID);
    if (container) {
      container.innerHTML = '';
    }
  }
  window.clearAllNotifications = clearAllNotifications;

  // Example usage (comment out in production):
  /*
  document.addEventListener('DOMContentLoaded', () => {
    showNotification("Welcome to Conqueror Engine!", { type: "success", duration: 3000 });
    showNotification("Loading modules...", { type: "info", duration: 4000 });
    showNotification("Warning: Low funds.", { type: "warning", duration: 5000, importance: 2 });
    showNotification("Error connecting to server!", { type: "error", duration: 6000, importance: 3 });
  });
  */
})();
