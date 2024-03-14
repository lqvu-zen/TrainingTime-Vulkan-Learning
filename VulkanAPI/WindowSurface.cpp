#include "WindowSurface.h"

#include <stdexcept>

namespace VulkanAPI
{
///////////////////////////////////////////////////////////////////////////////

WindowSurface::WindowSurface(VkInstance i_instance, GLFWwindow* i_window)
    : m_surface(nullptr)
    , m_instance(i_instance)
{
    if (glfwCreateWindowSurface(m_instance, i_window, nullptr, &m_surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}

WindowSurface::~WindowSurface()
{
}

VkSurfaceKHR WindowSurface::GetSurface()
{
    return m_surface;
}

void WindowSurface::Cleanup()
{
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
}

///////////////////////////////////////////////////////////////////////////////
}// VulkanAPI