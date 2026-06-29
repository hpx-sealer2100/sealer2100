package com.jubiter.sdk.demo.utils;

import android.os.Handler;
import android.os.Looper;
import android.util.Log;

import java.util.List;
import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicReference;

/**
 * @author fengshuo
 * @date 2018/4/26
 * @time 14:28
 */
public class ThreadUtils {
    private static final String TAG = "ThreadUtils";

    // 使用 AtomicReference 保证线程安全地更新线程池引用
    private static final AtomicReference<ExecutorService> executorRef = new AtomicReference<>();

    // 默认配置（可根据需要调整）
    private static final int DEFAULT_CORE_POOL_SIZE = Runtime.getRuntime().availableProcessors();
    private static final int DEFAULT_MAX_POOL_SIZE = DEFAULT_CORE_POOL_SIZE * 2;
    private static final long DEFAULT_KEEP_ALIVE_SECONDS = 60L;
    private static final int DEFAULT_QUEUE_CAPACITY = 1024;
    private static final String DEFAULT_THREAD_PREFIX = "SmartPool";


    /**
     * 创建并设置新的线程池（内部使用）
     */
    private static ExecutorService createThreadPool(int corePoolSize,
                                                    int maxPoolSize,
                                                    long keepAliveSeconds,
                                                    int queueCapacity,
                                                    String threadNamePrefix) {

        ThreadFactory threadFactory = new CustomThreadFactory(threadNamePrefix);
        BlockingQueue<Runnable> workQueue = new LinkedBlockingQueue<>(queueCapacity);

        return new ThreadPoolExecutor(
                corePoolSize,
                maxPoolSize,
                keepAliveSeconds,
                TimeUnit.SECONDS,
                workQueue,
                threadFactory,
                new ThreadPoolExecutor.CallerRunsPolicy() // 拒绝策略：由调用线程执行
        );
    }

    private static void createThreadPool() {
        ExecutorService newExecutor = createThreadPool(
                DEFAULT_CORE_POOL_SIZE,
                DEFAULT_MAX_POOL_SIZE,
                DEFAULT_KEEP_ALIVE_SECONDS,
                DEFAULT_QUEUE_CAPACITY,
                DEFAULT_THREAD_PREFIX
        );

        // 原子更新线程池引用
        ExecutorService oldExecutor = executorRef.getAndSet(newExecutor);

        // 如果旧的线程池不为null，应当关闭它
        if (oldExecutor != null && !oldExecutor.isTerminated() && !oldExecutor.isShutdown()) {
            gracefulShutdown(oldExecutor, 0);
        }
    }

    /**
     * 自动检查线程池状态，不可用时重建
     */
    public static void execute(Runnable task) {
        if (task == null) {
            throw new IllegalArgumentException("Task cannot be null");
        }

        ExecutorService executor = executorRef.get();

        // 如果线程池为空 或 已关闭，则重建
        if (executor == null || executor.isShutdown() || executor.isTerminated()) {
            synchronized (ThreadUtils.class) {
                // 双重检查
                executor = executorRef.get();
                if (executor == null || executor.isShutdown() || executor.isTerminated()) {
                    createThreadPool(); // 使用默认参数重建
                    executor = executorRef.get();
                }
            }
        }

        executor.submit(task);
    }

    /**
     * 终止并清理当前线程池
     */
    public static void shutdown() {
        ExecutorService executor = executorRef.get();
        if (executor != null) {
            executorRef.set(null); // 先断开引用
            gracefulShutdown(executor, 0);
        }
    }

    /**
     * 关闭线程池
     *
     * @param executor       要关闭的线程池
     * @param timeoutSeconds 等待超时时间，<=0时表示立即强制关闭
     */
    private static void gracefulShutdown(ExecutorService executor, int timeoutSeconds) {
        if (executor == null || executor.isTerminated()) return;

        if (timeoutSeconds <= 0) {
            // 如果超时时间小于等于0，直接强制关闭
            List<Runnable> remainingTasks = executor.shutdownNow();
            if (!remainingTasks.isEmpty()) {
                Log.d(TAG, "强制关闭线程池，剩余未执行任务数: " + remainingTasks.size());
            }
        } else {
            executor.shutdown();
            try {
                if (!executor.awaitTermination(timeoutSeconds, TimeUnit.SECONDS)) {
                    Log.w(TAG, "关闭超时，开始强制关闭");
                    List<Runnable> remainingTasks = executor.shutdownNow();
                    if (!executor.awaitTermination(timeoutSeconds, TimeUnit.SECONDS)) {
                        Log.e(TAG, "线程池强制关闭失败");
                    } else if (!remainingTasks.isEmpty()) {
                        Log.d(TAG, "强制关闭完成，剩余未执行任务数: " + remainingTasks.size());
                    }
                } else {
                    Log.d(TAG, "线程池关闭完成");
                }
            } catch (InterruptedException e) {
                Log.e(TAG, "线程池关闭过程中被中断");
                List<Runnable> remainingTasks = executor.shutdownNow();
                if (!remainingTasks.isEmpty()) {
                    Log.d(TAG, "中断后强制关闭，剩余未执行任务数: " + remainingTasks.size());
                }
                Thread.currentThread().interrupt();
            }
        }
    }

    /**
     * 自定义线程工厂
     */
    private static class CustomThreadFactory implements ThreadFactory {
        private final String prefix;
        private final AtomicInteger counter = new AtomicInteger(1);

        public CustomThreadFactory(String prefix) {
            this.prefix = prefix;
        }

        @Override
        public Thread newThread(Runnable r) {
            return new Thread(r, prefix + "-thread-" + counter.getAndIncrement());
        }
    }


    private static final Handler sMainHandler = new Handler(Looper.getMainLooper());

    public static void toMainThread(Runnable runnable) {
        sMainHandler.post(runnable);
    }

}
